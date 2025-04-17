#include <include/img2fft_gui.h>

//#include <src/savannah/platforms/opengl/opengl_texture.cpp>

//#define STB_IMAGE_IMPLEMENTATION
#include <external/stb_image.h>
#include <external/stb_image_write.h>
#include <filesystem>

const double pi = 3.14159265358979323846;
const double pi_half = pi/2;
const double pi_square = pi*pi;

namespace Savannah 
{	
	Image::Image()
	{}
	
	Image::~Image()
	{
		Unload();
		
		delete texture;
		texture = nullptr;
		
		delete originalTexture;
		originalTexture = nullptr;
		
		delete[] fftw_data;
		fftw_data = nullptr;
	}
	
	void Image::Load(const std::string& filepath)
	{
		// load pixels
		int w, h, c;
		pixels = stbi_load(filepath.c_str(), &w, &h, &c, channelsDesired);
		if (!pixels)
		{
			CONSOLE_LOG("stbi_load returned NULL. File format is probably not suppored.");
			return;
		} else {
			width = w;
			height = h;
			channelsOriginal = c;
		}
		
		aspectRatio = (float)height / width;
		CONSOLE_LOG("Loaded Image constraints (w, h, c, a/r): ", width, ", ", height, ", ", channelsOriginal, ", ", aspectRatio);
		CONSOLE_LOG("Desired channels: ", channelsDesired);
		
		// reinitialize values
		fftw_data_max = 0.0;
		fftw_data_min = 1.0;
		magnitudeOrderPolishCoefficient = 1.0;
		
		// load fftw data from pixels
		CONSOLE_LOG("Generating FFTW data");
		delete[] fftw_data;
		fftw_data = new fftw_complex[width * height];
		unsigned char* pixel = pixels;
		uint32_t p, r, g, b, a;
		p = 256 * 256 * 256 * 256 - 1;
		r = 256 * 256 * 256;
		g = 256 * 256;
		b = 256;
		a = 1;
		for (int x = 0; x < width; ++x)
		{
			int xIndex = x*height; 
			for (int yIndex = 0; yIndex < height; ++yIndex)
			{
				if (channelsOriginal == 4)
				{
					uint32_t value = *(pixels + (xIndex + yIndex) * channelsOriginal) * r + 
					*(pixels + (xIndex + yIndex) * channelsOriginal + 1) * g +
					*(pixels + (xIndex + yIndex) * channelsOriginal + 2) * b +
					*(pixels + (xIndex + yIndex) * channelsOriginal + 3) * a;
					
					fftw_data[xIndex + yIndex][0] = (float)value / p;
					
					if (colors.find(value) != colors.end())
					{
						colors[value] += 1;
					} else {
						colors[value] = 1;
					}
				}
				if (channelsOriginal == 3)
				{
					uint32_t value = *(pixels + (xIndex + yIndex) * channelsOriginal) * g + 
					*(pixels + (xIndex + yIndex) * channelsOriginal + 1) * b +
					*(pixels + (xIndex + yIndex) * channelsOriginal + 2) * a;
					
					fftw_data[xIndex + yIndex][0] = (float)value / r;
					
					if (colors.find(value) != colors.end())
					{
						colors[value] += 1;
					} else {
						colors[value] = 1;
					}
				}
			}
		}
		CONSOLE_LOG("FFTW data generation done");
		
		// load texture to display
		UpdateTexture();
		UpdateOriginalTexture();
		CONSOLE_LOG("Texture updated");
	}
	
	void Image::UpdatePixels(Img2FFTColorScheme* colorScheme, FourierSpectrumMode mode)
	{
		if (!pixels)
		{
			return;
		}
		if (!colorScheme)
		{
			CONSOLE_RED("Image::UpdatePixels: color scheme is nullptr");
			return;
		} else {
			CONSOLE_LOG("Chosen scheme: ", colorScheme->name);
		}
		magnitudeOrderPolishCoefficient = brightnessCoefficient*sqrt((fftw_data_max / fftw_data_min));
		magnitudeOrderPolishCoefficient = (magnitudeOrderPolishCoefficient == INFINITY) ? 100.0 : magnitudeOrderPolishCoefficient;		
		CONSOLE_LOG("FFTW Data. Max: ", fftw_data_max, "; Min: ", fftw_data_min);
		CONSOLE_LOG("Magnitude Order: ", magnitudeOrderPolishCoefficient);
		
		unsigned char* pixel = pixels;
		for (int x = 0; x < width; ++x)
		{
			int xIndex = x*height;
			int red, green, blue = 0;
			
			for (int yIndex = 0; yIndex < height; ++yIndex)
			{
				double value = 0.0;
				
				switch (mode)
				{
					case FourierSpectrumMode::Amplitude:
						{
							value = sqrt(fftw_data[xIndex + yIndex][0] * fftw_data[xIndex + yIndex][0] + fftw_data[xIndex + yIndex][1] * fftw_data[xIndex + yIndex][1]);
							value = (value * magnitudeOrderPolishCoefficient < 1.0) ? value * magnitudeOrderPolishCoefficient : 1.0;
						}
						break;
					case FourierSpectrumMode::Phase:
						{
							if (fftw_data[xIndex + yIndex][0] == 0)
							{
								if (fftw_data[xIndex + yIndex][1] >= 0)
								{
									value = pi_half;
								} else {
									value = -pi_half;
								}
							} else {
								value = atan(fftw_data[xIndex + yIndex][1] / fftw_data[xIndex + yIndex][0]);
							}
							
							value = (value + pi_half) / pi;
						}
						break;
					case FourierSpectrumMode::Real:
						{
							value = fftw_data[xIndex + yIndex][0];
							value = (value * magnitudeOrderPolishCoefficient < 1.0) ? value * magnitudeOrderPolishCoefficient : 1.0;
						}
						break;
					case FourierSpectrumMode::Imaginary:
						{
							value = fftw_data[xIndex + yIndex][1];
							value = (value * magnitudeOrderPolishCoefficient < 1.0) ? value * magnitudeOrderPolishCoefficient : 1.0;
						}
						break;
				}
				
				colorScheme->GetColor(value, fftw_data_min, fftw_data_max, &red, &green, &blue);
				
				*(pixels + (xIndex + yIndex) * channelsOriginal) = red;
				*(pixels + (xIndex + yIndex) * channelsOriginal + 1) = green;
				*(pixels + (xIndex + yIndex) * channelsOriginal + 2) = blue;
				if (channelsOriginal == 4)
					*(pixels + (xIndex + yIndex) * channelsOriginal + 3) = 255; // alpha
			}
		}
	}
	
	void Image::UpdateTexture()
	{
		if (!pixels)
		{
			return;
		}
		delete texture;
		texture = new OpenGLTexture2D(pixels, width, height, channelsOriginal);
	}
	
	void Image::UpdateOriginalTexture()
	{
		if (!pixels)
		{
			return;
		}
		delete originalTexture;
		originalTexture = new OpenGLTexture2D(pixels, width, height, channelsOriginal);
	}
	
	void Image::NormalizeFFT()
	{
		// update fftw data absolute min and max metrics
		fftw_data_max = 0.0; // absolute value
		fftw_data_min = 1.0; // absolute value
		for (int x = 0; x < width; ++x)
		{
			for (int y = 0; y < height; ++y)
			{
				double value;
				
				value = fabs(fftw_data[x*height + y][0]);
				if ((value > fftw_data_max) && value != 1.0)
					fftw_data_max = value;
				if ((value < fftw_data_min) && value != 0.0)
					fftw_data_min = value;
				
				value = fabs(fftw_data[x*height + y][1]);
				if ((value > fftw_data_max) && value != 1.0)
					fftw_data_max = value;
				if ((value < fftw_data_min) && value != 0.0)
					fftw_data_min = value;
			}
		}
		
		// normalize
		for (int x = 0; x < width; ++x)
		{
			for (int y = 0; y < height; ++y)
			{
				fftw_data[x*height + y][0] = fftw_data[x*height + y][0] / fftw_data_max;
				fftw_data[x*height + y][1] = fftw_data[x*height + y][1] / fftw_data_max;
			}
		}
		
		fftw_data_max = 1.0;
	}
	
	void Image::Unload()
	{
		if (pixels)
		{
			stbi_image_free(pixels);
			pixels = nullptr;
			colors.clear();
			colorsSorted.clear();
//			width = 0;
//			height = 0;
//			channelsOriginal = 0;
//			aspectRatio = 1.0;
		}
	}
	
	void Image::Save(const std::string& path)
	{
		CONSOLE_LOG("stbi call to write png");
		int stride_in_bytes = width * channelsOriginal;
		stbi_write_png(path.c_str(), width, height, channelsOriginal, pixels, stride_in_bytes);
	}
	
	
	void Img2FFTColorScheme::GetColor(double value, double min, double max, int* r, int* g, int* b)
	{
		int left, right;
//		CONSOLE_LOG(colors.size());
		
		if (value <= min)
		{
			left = 0;
			right = 0;
		} else if (value >= max) {
			left = colors.size() - 1;
			right = colors.size() - 1;
		} else {
			// "move" value across the color map into appropriate region
			value = value * (colors.size() - 1);
			left = floor(value);
			right = left + 1;
			value = value - (float)left;
		}
		
		*r = (colors[right].x - colors[left].x) * value + colors[left].x;
		*g = (colors[right].y - colors[left].y) * value + colors[left].y;
		*b = (colors[right].z - colors[left].z) * value + colors[left].z;
		
		return;
	}
	
	Img2FFT::Img2FFT()
	{
		CONSOLE_GREEN("Application initialized.");
		CONSOLE_LOG("Max texture size is: ", GL_MAX_TEXTURE_SIZE);
		
		SetWindowTitle("Img 2 Fourier - Визуализатор БПФ по изображению");
		
		m_fileDialogInfo.type = ImGuiFileDialogType_OpenFile;
		m_fileDialogInfo.title = "Открыть файл";
		m_fileDialogInfo.fileName = "";
		m_fileDialogInfo.directoryPath = std::filesystem::current_path();
		m_fileDialogInfo.fileExtensions = {
			".PNG", 
			".jpg", 
			".bmp", 
			".gif",
		};
		m_fileDialogInfo.fileExtensionSelected = -1;
		
		m_ColorSchemesMap["Greenish"] = {
			{  0,   0,   0, 255},
			{196, 255, 128, 255},
			{255, 255, 255, 255}
		};
		m_ColorSchemesNamesList.push_back("Greenish");
		
		m_ColorSchemesMap["Matrix"] = {
			{  0,   0,   0, 255},
			{  0, 255,   0, 255},
			{  0, 255, 255, 255},
			{255, 255, 255, 255}
		};
		m_ColorSchemesNamesList.push_back("Matrix");
		
		m_ColorSchemesMap["Black & White"] = {
			{  0,   0,   0, 255},
			{255, 255, 255, 255}
		};
		m_ColorSchemesNamesList.push_back("Black & White");
		
		m_ColorSchemesMap["White & Black"] = {
			{255, 255, 255, 255},
			{  0,   0,   0, 255}
		};
		m_ColorSchemesNamesList.push_back("White & Black");
		
		m_ColorSchemesMap["Jet with B&W"] = {
			{  0,   0,   0, 255},
			{  0,   0, 255, 255},
			{  0, 255, 255, 255},
			{  0, 255,   0, 255},
			{255, 255,   0, 255},
			{255,   0,   0, 255},
			{255, 255, 255, 255}
		};
		m_ColorSchemesNamesList.push_back("Jet with B&W");
		
		m_ColorSchemesMap["Jet"] = {
			{  0,   0,  64, 255},
			{  0,   0, 255, 255},
			{  0, 255, 255, 255},
			{  0, 255,   0, 255},
			{255, 255,   0, 255},
			{255,   0,   0, 255},
			{ 64,   0,   0, 255}
		};
		m_ColorSchemesNamesList.push_back("Jet");
		
		m_ColorSchemesMap["HSV"] = {
			{255,   0,   0, 255},
			{255, 255,   0, 255},
			{  0, 255,   0, 255},
			{  0, 255, 255, 255},
			{  0,   0, 255, 255},
			{255,   0, 255, 255},
			{255,   0,   0, 255}
		};
		m_ColorSchemesNamesList.push_back("HSV");
		
		m_ColorSchemesMap["Inferno"] = {
			{  0,   0,   0, 255},
			{255,   0, 255, 255},
			{255, 127, 127, 255},
			{255, 255,   0, 255},
			{255, 255, 255, 255}
		};
		m_ColorSchemesNamesList.push_back("Inferno");
		
		m_ColorSchemesMap["Summer"] = {
			{  0, 127, 127, 255},
			{255, 255,   0, 255},
		};
		m_ColorSchemesNamesList.push_back("Summer");
		
		m_ColorSchemesMap["Seismic"] = {
			{  0,   0,   0, 255},
			{  0,   0, 255, 255},
			{255, 255, 255, 255},
			{255,   0,   0, 255},
			{127,   0,   0, 255},
		};
		m_ColorSchemesNamesList.push_back("Seismic");
		
		m_ColorSchemesMap["Hot"] = {
			{  0,   0,   0, 255},
			{255,   0,   0, 255},
			{255, 255,   0, 255},
			{255, 255, 255, 255}
		};
		m_ColorSchemesNamesList.push_back("Hot");
		
		m_ColorSchemesMap["Blue Space"] = {
			{  0,   0,   0, 255},
			{  0,   0, 128, 255},
			{  0,   0, 255, 255},
			{  0, 128, 256, 255},
			{ 64, 255,  64, 255},
			{255, 255, 255, 255},
		};
		m_ColorSchemesNamesList.push_back("Blue Space");
		
		m_ColorSchemesMap["Native"] = {
			{  0,   0,   0, 255}, // 0 // can be thrown away
			{  0,   0,  63, 255}, // 1
			{  0,   0, 127, 255},
			{  0,   0, 191, 255},
			{  0,   0, 255, 255},
			{  0,  63,   0, 255},
			{  0,  63,  63, 255},
			{  0,  63, 127, 255},
			{  0,  63, 191, 255},
			{  0,  63, 255, 255}, 
			{  0, 127,   0, 255}, // 10
			{  0, 127,  63, 255},
			{  0, 127, 127, 255},
			{  0, 127, 191, 255},
			{  0, 127, 255, 255},
			{  0, 191,   0, 255},
			{  0, 191,  63, 255},
			{  0, 191, 127, 255},
			{  0, 191, 191, 255},
			{  0, 191, 255, 255},
			{  0, 255,   0, 255}, // 20
			{  0, 255,  63, 255},
			{  0, 255, 127, 255},
			{  0, 255, 191, 255},
			{  0, 255, 255, 255},
			{ 63,   0,   0, 255},
			{ 63,   0,  63, 255},
			{ 63,   0, 127, 255},
			{ 63,   0, 191, 255},
			{ 63,   0, 255, 255},
			{ 63,  63,   0, 255}, // 30
			{ 63,  63,  63, 255},
			{ 63,  63, 127, 255},
			{ 63,  63, 191, 255},
			{ 63,  63, 255, 255},
			{ 63, 127,   0, 255},
			{ 63, 127,  63, 255},
			{ 63, 127, 127, 255},
			{ 63, 127, 191, 255},
			{ 63, 127, 255, 255},
			{ 63, 191,   0, 255}, // 40
			{ 63, 191,  63, 255},
			{ 63, 191, 127, 255},
			{ 63, 191, 191, 255},
			{ 63, 191, 255, 255},
			{ 63, 255,   0, 255},
			{ 63, 255,  63, 255},
			{ 63, 255, 127, 255},
			{ 63, 255, 191, 255},
			{ 63, 255, 255, 255},
			{127,   0,   0, 255}, // 50
			{127,   0,  63, 255},
			{127,   0, 127, 255},
			{127,   0, 191, 255},
			{127,   0, 255, 255},
			{127,  63,   0, 255},
			{127,  63,  63, 255},
			{127,  63, 127, 255},
			{127,  63, 191, 255},
			{127,  63, 255, 255},
			{127, 127,   0, 255}, // 60
			{127, 127,  63, 255},
			{127, 127, 127, 255},
			{127, 127, 191, 255},
			{127, 127, 255, 255},
			{127, 191,   0, 255},
			{127, 191,  63, 255},
			{127, 191, 127, 255},
			{127, 191, 191, 255},
			{127, 191, 255, 255},
			{127, 255,   0, 255}, // 70
			{127, 255,  63, 255},
			{127, 255, 127, 255},
			{127, 255, 191, 255},
			{127, 255, 255, 255},
			{191,   0,   0, 255},
			{191,   0,  63, 255},
			{191,   0, 127, 255},
			{191,   0, 191, 255},
			{191,   0, 255, 255},
			{191,  63,   0, 255}, // 80
			{191,  63,  63, 255},
			{191,  63, 127, 255},
			{191,  63, 191, 255},
			{191,  63, 255, 255},
			{191, 127,   0, 255},
			{191, 127,  63, 255},
			{191, 127, 127, 255},
			{191, 127, 191, 255},
			{191, 127, 255, 255},
			{191, 191,   0, 255}, // 90
			{191, 191,  63, 255},
			{191, 191, 127, 255},
			{191, 191, 191, 255},
			{191, 191, 255, 255},
			{191, 255,   0, 255},
			{191, 255,  63, 255},
			{191, 255, 127, 255},
			{191, 255, 191, 255},
			{191, 255, 255, 255},
			{255,   0,   0, 255}, // 100
			{255,   0,  63, 255},
			{255,   0, 127, 255},
			{255,   0, 191, 255},
			{255,   0, 255, 255},
			{255,  63,   0, 255},
			{255,  63,  63, 255},
			{255,  63, 127, 255},
			{255,  63, 191, 255},
			{255,  63, 255, 255},
			{255, 127,   0, 255}, // 110
			{255, 127,  63, 255},
			{255, 127, 127, 255},
			{255, 127, 191, 255},
			{255, 127, 255, 255},
			{255, 191,   0, 255},
			{255, 191,  63, 255},
			{255, 191, 127, 255},
			{255, 191, 191, 255},
			{255, 191, 255, 255},
			{255, 255,   0, 255}, // 120
			{255, 255,  63, 255},
			{255, 255, 127, 255},
			{255, 255, 191, 255},
			{255, 255, 255, 255}, // 124
		};
		m_ColorSchemesNamesList.push_back("Native");
		
		m_ColorSchemeSelected = 0;
		m_ColorSchemeSelectedName = m_ColorSchemesNamesList[m_ColorSchemeSelected];
		
		for (auto it = m_ColorSchemesMap.begin(); it != m_ColorSchemesMap.end(); ++it)
		{
			m_ColorSchemes[it->first] = {it->first, {}};
			for (int i = 0; i < m_ColorSchemesMap[it->first].size(); ++i)
			{
				m_ColorSchemes[it->first].colors.push_back(m_ColorSchemesMap[it->first][i]);
			}
			CONSOLE_LOG(it->first, " size: ", m_ColorSchemes[it->first].colors.size());
		}
		
		m_CurrentMode = Img2FFTMode::Idle;
		CONSOLE_LOG("Enter Idle mode");
		
		NewTask(Img2FFTTasks::Idle);
		CONSOLE_LOG("Add a new Task: Idle");
		
		// setup File Dialog initial values
		m_fileDialogInfo.type = ImGuiFileDialogType_OpenFile;
		m_fileDialogInfo.title = "Открыть файл";
		m_fileDialogInfo.fileName = "";
		m_fileDialogInfo.directoryPath = std::filesystem::current_path();
		m_fileDialogInfo.fileExtensions = {
			".PNG", 
			".jpg", 
			".bmp", 
			".gif",
		};
		m_fileDialogInfo.fileExtensionSelected = -1;
	}
	
	Img2FFT::~Img2FFT()
	{
		delete m_Image;
		m_Image = nullptr;
		
		FFTClear();
		CONSOLE_GREEN("Application is shut down.");
	}
	
	void Img2FFT::SetupResources()
	{
		// image stuff
		m_Image = new Image();
		m_Image->Load("data/img/temp.png");
		
		// fftw stuff
		FFTPlan();
		CONSOLE_LOG("FFTW plan created");
	}
	
	void Img2FFT::Logic()
	{
		while (m_TaskStack.size() > 0)
		{
			m_CurrentTask = m_TaskStack.back();
			m_TaskStack.pop_back();
			
			switch (m_CurrentTask)
			{
			case Img2FFTTasks::Idle:
				// do nothing
				break;
			case Img2FFTTasks::CalculateFFT:
				{
					if (m_fftw_planned)
					{
						// FFTW across width
//						m_Image->NormalizeFFT();
						for (int y = 0; y < m_Image->height; ++y)
						{
							for (int x = 0; x < m_Image->width; ++x)
							{
								m_fftw_in_width[x][0] = m_Image->fftw_data[x + y*m_Image->width][0];
								m_fftw_in_width[x][1] = m_Image->fftw_data[x + y*m_Image->width][1];
							}
							
							fftw_execute(m_fftw_plan_width);
							
							// center fft
							int offset = m_Image->width / 2;
							for (int x = 0; x < m_Image->width; ++x)
							{
								// IMPORTANT: This works only if m_Image->width is even
								if (x < offset)
								{
									m_Image->fftw_data[x + y*m_Image->width][0] = m_fftw_out_width[offset + x][0];
									m_Image->fftw_data[x + y*m_Image->width][1] = m_fftw_out_width[offset + x][1];
								} else {
									m_Image->fftw_data[x + y*m_Image->width][0] = m_fftw_out_width[x - offset][0];
									m_Image->fftw_data[x + y*m_Image->width][1] = m_fftw_out_width[x - offset][1];
								}
							}
						}
						
						// FFTW across height
//						m_Image->NormalizeFFT();
						for (int x = 0; x < m_Image->width; ++x)
						{
							for (int y = 0; y < m_Image->height; ++y)
							{
								m_fftw_in_height[y][0] = m_Image->fftw_data[x + y*m_Image->width][0];
								m_fftw_in_height[y][1] = m_Image->fftw_data[x + y*m_Image->width][1];
							}
							
							fftw_execute(m_fftw_plan_height);
							
							int offset = m_Image->height / 2;
//							int offset = 0;
							for (int y = 0; y < m_Image->height; ++y)
							{
								if (y < offset)
								{
									m_Image->fftw_data[x + y*m_Image->width][0] = m_fftw_out_height[offset + y][0];
									m_Image->fftw_data[x + y*m_Image->width][1] = m_fftw_out_height[offset + y][1];
								} else {
									m_Image->fftw_data[x + y*m_Image->width][0] = m_fftw_out_height[y - offset][0];
									m_Image->fftw_data[x + y*m_Image->width][1] = m_fftw_out_height[y - offset][1];
								}
							}
						}
						
						
						m_Image->NormalizeFFT();
						lowerFFTWLevel = m_Image->fftw_data_min;
						CONSOLE_LOG("FFTW max: ", m_Image->fftw_data_max, "; FFTW min: ", m_Image->fftw_data_min);
						m_Image->UpdatePixels(&m_ColorSchemes[m_ColorSchemeSelectedName], m_FourierSpectrumMode);
						m_Image->UpdateTexture();
						
						m_FFTCalculated = true;
					}
				}
				break;
			case Img2FFTTasks::LoadImage:
				{
					CONSOLE_LOG("Loading image from path: ", m_fileDialogInfo.resultPath.string());
					m_Image->Unload();
					m_Image->Load(m_fileDialogInfo.resultPath.string());
					
					lowerFFTWLevel = m_Image->fftw_data_min;
					FFTClear();
					FFTPlan();
					
//					// load a custom color scheme
//					uint32_t bestColor = 0;
//					CONSOLE_LOG("Total colors: ", m_Image->colors.size());
//					
//					m_Image->colorsSorted.clear();
//					m_Image->colorsSorted.reserve(m_Image->colors.size());
//					for (auto it = m_Image->colors.begin(); it != m_Image->colors.end(); ++it)
//					{
//						m_Image->colorsSorted.push_back({it->first, it->second});
//					}
//					std::sort(m_Image->colorsSorted.begin(), m_Image->colorsSorted.end(), 
//						[](const std::pair<uint32_t, uint32_t>& a, const std::pair<uint32_t, uint32_t>& b){
//							return a.second < b.second;
//						});
//					CONSOLE_LOG("Sorted colors");
//					
//					m_ColorSchemes["Native"].colors.clear();
//					m_ColorSchemes["Native"].colors.push_back({0, 0, 0, 0});
//					uint8_t red, green, blue = 0;
//					int colorsMax = 10;
//					for (int i = 0; (i < colorsMax) && (i < m_Image->colorsSorted.size()); ++i)
//					{
//						bestColor = m_Image->colorsSorted[m_Image->colorsSorted.size() - 1 - i].second;
//						uint8_t* look = (uint8_t*)(&bestColor);
//						red = *look;
//						green = *(look + 1);
//						blue = *(look + 2);
//						m_ColorSchemes["Native"].colors.push_back({red, green, blue, 255});
//					}
//					m_ColorSchemes["Native"].colors.push_back({255, 255, 255, 255});
//					CONSOLE_LOG("Colors in Native colormap: ", m_ColorSchemes["Native"].colors.size());
//					for (int k = 0; k < m_ColorSchemes["Native"].colors.size(); ++k)
//					{
//						CONSOLE_LOG("Native color #", k, 
//							": r:", m_ColorSchemes["Native"].colors[k].x, 
//							", g:", m_ColorSchemes["Native"].colors[k].y, 
//							", b:", m_ColorSchemes["Native"].colors[k].z, 
//							", a:", m_ColorSchemes["Native"].colors[k].w);
//					}
					
					m_FFTCalculated = false;
				}
				break;
			case Img2FFTTasks::SaveFFT:
				{
					CONSOLE_LOG("Saving FFT image to path: ", m_fileDialogInfo.resultPath.string());
					m_Image->Save(m_fileDialogInfo.resultPath.string());
				}
				break;
			case Img2FFTTasks::Exit:
				{
					doExit = true;
				}
				break;
			default:
				break;
			}
		}
		m_CurrentTask = Img2FFTTasks::Idle;
	}
	
	void Img2FFT::GUIContent() 
	{
		bool p_open = true;
		static bool use_work_area = true;
		// static ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings;
		static ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoSavedSettings;
		
		TEXT_BASE_WIDTH = ImGui::CalcTextSize("A").x * m_WindowScale.x;
		TEXT_BASE_HEIGHT = ImGui::GetTextLineHeightWithSpacing() * m_WindowScale.y;
//		CONSOLE_LOG("Window scale: (", m_WindowScale.x, ", ", m_WindowScale.y, ")");
		
		// ====================================================================================
		// The Application starts here
		// ------------------------------------------------------------------------------------
		
		ShowMainMenu();
		
		// We demonstrate using the full viewport area or the work area (without menu-bars, task-bars etc.)
		// Based on your use case you may want one or the other.
		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(use_work_area ? viewport->WorkPos : viewport->Pos);
		ImGui::SetNextWindowSize(use_work_area ? viewport->WorkSize: viewport->Size);
		// ------------------------------------------------------------------------------------
		
		if (ImGui::Begin("Example: Fullscreen window", &p_open, flags))
		{

			ImGui::Columns(2);
			static float columnWidth1 = 3 * m_WindowCurrentWidth / 4; // 960
			static float columnWidth2 = 1 * m_WindowCurrentWidth / 4; // 320
			
			ImGui::SetColumnWidth(0, columnWidth1*m_WindowScale.x);
			ImGui::SetColumnWidth(1, columnWidth2*m_WindowScale.y);
			
			static float columnHeight = ImGui::GetContentRegionAvail().y - 20;
			
			ImGui::Image((ImTextureID)(m_Image->texture->GetID()),
				ImVec2(columnWidth1*m_WindowScale.x, columnHeight*m_WindowScale.y),
				ImVec2(0, 0),
				ImVec2(1, 1));
			
			ImGui::NextColumn();
			ImGui::SeparatorText("Настройка усиления и цветовой схемы");
			ImGui::Text("Нижний порог: ");
			ImGui::SameLine(150);
			ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
			if (ImGui::InputFloat("###lowerFFT", &lowerFFTWLevel, 0.0000001, 0.001, "%.10f"))
			{
				if (lowerFFTWLevel < 0.0)
				{
					lowerFFTWLevel = 0.0;
				}
				m_Image->fftw_data_min = lowerFFTWLevel;
				m_Image->UpdatePixels(&m_ColorSchemes[m_ColorSchemeSelectedName], m_FourierSpectrumMode);
				m_Image->UpdateTexture();
			}
			
			ImGui::Text("Верхний порог: ");
			ImGui::SameLine(150);
			ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
			if (ImGui::InputFloat("###upperFFT", &upperFFTWlevel, 0.0000001, 0.001, "%.10f"))
			{
				if (upperFFTWlevel < 0.0)
				{
					upperFFTWlevel = 0.0;
				}
				m_Image->fftw_data_max = upperFFTWlevel;
				m_Image->UpdatePixels(&m_ColorSchemes[m_ColorSchemeSelectedName], m_FourierSpectrumMode);
				m_Image->UpdateTexture();
			}
			
			ImGui::Text("Раскраска: ");
			ImGui::SameLine(150);
			ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
			if (ImGui::BeginCombo("###ColorSchemeSelector", m_ColorSchemesNamesList[m_ColorSchemeSelected].c_str()))
			{
				static bool isSelected = false;
				for (int i = 0; i < m_ColorSchemesNamesList.size(); ++i)
				{
					isSelected = (i == m_ColorSchemeSelected);
					if (ImGui::Selectable(m_ColorSchemesNamesList[i].c_str(), isSelected))
					{
						m_ColorSchemeSelected = i;
						m_ColorSchemeSelectedName = m_ColorSchemesNamesList[i];
						m_Image->UpdatePixels(&m_ColorSchemes[m_ColorSchemeSelectedName], m_FourierSpectrumMode);
						m_Image->UpdateTexture();
					}
				}
				ImGui::EndCombo();
			}
			
			ImGui::Separator();
			ImGui::Text("Вид частотного спектра:"); 
			ImGui::SameLine();
			switch (m_FourierSpectrumMode)
			{
				case FourierSpectrumMode::Amplitude:
					{
						ImGui::TextColored({1.0, 0.8, 0.2, 1.0}, "Амплитудный");
					}
					break;
				case FourierSpectrumMode::Phase:
					{
						ImGui::TextColored({1.0, 0.8, 0.2, 1.0}, "Фазовый");
					}
					break;
				case FourierSpectrumMode::Real:
					{
						ImGui::TextColored({1.0, 0.8, 0.2, 1.0}, "Действительный");
					}
					break;
				case FourierSpectrumMode::Imaginary:
					{
						ImGui::TextColored({1.0, 0.8, 0.2, 1.0}, "Мнимый");
					}
					break;
			}
			
			ImGui::PushStyleColor(ImGuiCol_Button, {0.5, 0, 0.25, 1.0});
			if (ImGui::Button("Амплитудный", {150 * m_WindowScale.x, 30 * m_WindowScale.y}))
			{
				m_FourierSpectrumMode = FourierSpectrumMode::Amplitude;
				if (!m_FFTCalculated)
				{
					NewTask(Img2FFTTasks::CalculateFFT);
				} else {
					m_Image->UpdatePixels(&m_ColorSchemes[m_ColorSchemeSelectedName], m_FourierSpectrumMode);
					m_Image->UpdateTexture();
				}
			}
			ImGui::PopStyleColor();
			
			ImGui::SameLine(170 * m_WindowScale.x);
			
			ImGui::PushStyleColor(ImGuiCol_Button, {0.25, 0, 0.5, 1.0});
			if (ImGui::Button("Фазовый", {150 * m_WindowScale.x, 30 * m_WindowScale.y}))
			{
				m_FourierSpectrumMode = FourierSpectrumMode::Phase;
				if (!m_FFTCalculated)
				{
					NewTask(Img2FFTTasks::CalculateFFT);
				} else {
					m_Image->UpdatePixels(&m_ColorSchemes[m_ColorSchemeSelectedName], m_FourierSpectrumMode);
					m_Image->UpdateTexture();
				}
			}
			ImGui::PopStyleColor();
			
			ImGui::PushStyleColor(ImGuiCol_Button, {0.25, 0, 0.25, 1.0});
			if (ImGui::Button("Действительный", {150 * m_WindowScale.x, 30 * m_WindowScale.y}))
			{
				m_FourierSpectrumMode = FourierSpectrumMode::Real;
				if (!m_FFTCalculated)
				{
					NewTask(Img2FFTTasks::CalculateFFT);
				} else {
					m_Image->UpdatePixels(&m_ColorSchemes[m_ColorSchemeSelectedName], m_FourierSpectrumMode);
					m_Image->UpdateTexture();
				}
			}
			ImGui::PopStyleColor();
			
			ImGui::SameLine(170 * m_WindowScale.x);
			
			ImGui::PushStyleColor(ImGuiCol_Button, {0.25, 0.25, 0.5, 1.0});
			if (ImGui::Button("Мнимый", {150 * m_WindowScale.x, 30 * m_WindowScale.y}))
			{
				m_FourierSpectrumMode = FourierSpectrumMode::Imaginary;
				if (!m_FFTCalculated)
				{
					NewTask(Img2FFTTasks::CalculateFFT);
				} else {
					m_Image->UpdatePixels(&m_ColorSchemes[m_ColorSchemeSelectedName], m_FourierSpectrumMode);
					m_Image->UpdateTexture();
				}
			}
			ImGui::PopStyleColor();
			
			ImGui::SeparatorText("Оригинальное изображение");
			ImGui::Image((ImTextureID)(m_Image->originalTexture->GetID()),
				ImVec2(columnWidth2*m_WindowScale.x, columnWidth2*m_WindowScale.y * m_Image->aspectRatio),
				ImVec2(0, 0),
				ImVec2(1, 1));
			
			ImGui::End();
		}
		// ====================================================================================
	}
	
	void Img2FFT::NewTask(Img2FFTTasks task)
	{
		m_ErrorFlag = false;
		m_ErrorMessage = "";
		m_TaskStack.push_back(task);
	}
	
	void Img2FFT::ShowMainMenu()
	{
		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("Меню"))
			{
				if (ImGui::MenuItem("Открыть изображение...")) 
				{
					
					CONSOLE_LOG("Ask to open popup");
					m_fileDialogInfo.type = ImGuiFileDialogType_OpenFile;
					m_fileDialogInfo.title = "Открыть файл";
					m_fileDialogInfo.fileName = "";
					m_fileDialogInfo.fileExtensions = {
						".PNG", 
						".jpg", 
						".bmp", 
						".gif",
					};
					m_fileDialogInfo.fileExtensionSelected = -1;
					m_fileDialogOpen = true;
				}

				ImGui::Separator();
				if (ImGui::MenuItem("Сохранить изображение..."))
				{
					CONSOLE_LOG("Ask to open popup");
					m_fileDialogInfo.type = ImGuiFileDialogType_SaveFile;
					m_fileDialogInfo.title = "Сохранить файл";
					m_fileDialogInfo.fileName = "fft_image.png";
					m_fileDialogInfo.fileExtensions = {
						".png", 
						".jpg", 
						".bmp", 
						".gif",
					};
					m_fileDialogInfo.fileExtensionSelected = -1;
					m_fileDialogOpen = true;
					CONSOLE_LOG("Add a new Task: Save FFT image");
				}
				ImGui::Separator();
				if (ImGui::MenuItem("Выход")) 
				{
					NewTask(Img2FFTTasks::Exit);
					CONSOLE_LOG("Add a new Task: Exit");
				}
				ImGui::EndMenu();
			}
			
			if (ImGui::MenuItem("Фурье-образ"))
			{
				NewTask(Img2FFTTasks::CalculateFFT);
				CONSOLE_LOG("Add a new Task: Calculate Fourier Transform");
			}
			
			ImGui::EndMainMenuBar();
		}
		
		if (m_fileDialogOpen)
		{
			switch (m_fileDialogInfo.type)
			{
			case ImGuiFileDialogType_OpenFile:
				{
					if (ImGui::FileDialog(&m_fileDialogOpen, &m_fileDialogInfo))
					{
						// Result path in: m_fileDialogInfo.resultPath
						NewTask(Img2FFTTasks::LoadImage);
						CONSOLE_LOG("Add a new Task: Load an image");
						m_fileDialogOpen = false;
						ImGui::CloseCurrentPopup();
					}
				}
				break;
			case ImGuiFileDialogType_SaveFile:
				{
					if (ImGui::FileDialog(&m_fileDialogOpen, &m_fileDialogInfo))
					{
						// Result path in: m_fileDialogInfo.resultPath
						NewTask(Img2FFTTasks::SaveFFT);
						CONSOLE_LOG("Add a new Task: Save an FFT image");
						m_fileDialogOpen = false;
						ImGui::CloseCurrentPopup();
					}
				}
				break;
			}
		}
	}
	
	void Img2FFT::ShowContent()
	{
	}

	void Img2FFT::FFTClear()
	{
		if (m_fftw_planned)
		{
			fftw_destroy_plan(m_fftw_plan_height);
			fftw_destroy_plan(m_fftw_plan_width);
			fftw_free(m_fftw_out_height);
			fftw_free(m_fftw_out_width);
			fftw_free(m_fftw_in_height);
			fftw_free(m_fftw_in_width);
			
			m_fftw_in_width = nullptr;
			m_fftw_in_height = nullptr;
			m_fftw_out_width = nullptr;
			m_fftw_out_height = nullptr;
			
			m_fftw_planned = false;
		}
	}
	
	void Img2FFT::FFTPlan()
	{
		FFTClear();
		
		if (m_Image != nullptr)
		{
			if ((m_Image->width > 0) &&
				(m_Image->height > 0))
			{
				m_fftw_in_width = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * m_Image->width);
				m_fftw_in_height = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * m_Image->height);
				m_fftw_out_width = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * m_Image->width);
				m_fftw_out_height = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * m_Image->height);
				m_fftw_plan_width = fftw_plan_dft_1d(m_Image->width, m_fftw_in_width, m_fftw_out_width, FFTW_FORWARD, FFTW_ESTIMATE);
				m_fftw_plan_height = fftw_plan_dft_1d(m_Image->height, m_fftw_in_height, m_fftw_out_height, FFTW_FORWARD, FFTW_ESTIMATE);
				
				m_fftw_planned = true;
			}
		}
	}
	
	App* CreateApplication()
	{
		Img2FFT* app = new Img2FFT();
		app->SetFPS(SAVANNAH_FPS60);
		return app;
	}
}
