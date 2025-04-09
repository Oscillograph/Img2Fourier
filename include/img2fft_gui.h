#include <include/savannah/savannah.h>

// #include <include/yaml_wrapper.h>
#include <include/logger.h>
//#include <initializer_list>

#include <external/fftw/fftw3.h> // to calculate FFT

enum class FourierSpectrumMode
{
	Amplitude,
	Phase,
	Real,
	Imaginary
};

namespace Savannah 
{
	struct Img2FFTColorScheme
	{
		std::string name = "";
		std::vector<ImVec4> colors = {};
		
		void GetColor(double value, double min, double max, int* r, int* g, int* b);
	};
	
	class Image
	{
	public:
		Image();
		~Image();
		
		void Load(const std::string& filepath);
//		void BeforeUpdatePixels(); // adjust brightness 
		void UpdatePixels(Img2FFTColorScheme* colorScheme, FourierSpectrumMode mode = FourierSpectrumMode::Amplitude); // align pixels data with fftw data
		void UpdateTexture(); // if pixels were manipulated
		void UpdateOriginalTexture(); // if an image was loaded
		void NormalizeFFT();
		void Unload();
		void Save(const std::string& path);
		
		int width = 0;
		int height = 0;
		float aspectRatio = 1.0;
		int channelsOriginal = 0; // bit per pixel
		int channelsDesired = 0;
		double brightnessCoefficient = 2.0; // can be set up by user
		double magnitudeOrderPolishCoefficient = 1.0; // calculated dynamically by the app
		std::string path = "";
		std::unordered_map<uint32_t, uint32_t> colors = {};
		std::vector<std::pair<uint32_t, uint32_t>> colorsSorted = {};
		
		fftw_complex* fftw_data = nullptr;
		double fftw_data_max = 0.0; // absolute value
		double fftw_data_min = 0.0; // absolute value
		unsigned char* pixels = nullptr;
		OpenGLTexture2D* texture = nullptr;
		OpenGLTexture2D* originalTexture = nullptr;
		
	};
	
	// basically, commands to process
	enum class Img2FFTTasks : int
	{
		Idle						,
		
		LoadImage					,
		ShowImage					,
		HideImage					,
		UnloadImage					,
		
		CalculateFFT				,
		ShowFFT						,
		HideFFT						,
		SaveFFT						,
		
		ApplyColorMap				,
		ChangeColorMap				,
		
		Exit						
	};
	
	// basically, states which define what to draw on screen
	enum class Img2FFTMode : int
	{
		Idle						,
		
		CalculateFFT				,
		
		LoadImage					,
		ShowImage					,
		ShowFFT						,
		SaveFFT						,
		
		ChangeColorMap				,
		
		Exit
		
	};
	
	class Img2FFT : public App 
	{
	public:
		Img2FFT();
		~Img2FFT();
		
		// General Savannah App interface
		// void PreSetup() override;
		void SetupResources() override;
		void Logic() override;
		void GUIContent() override;
		
		// Img2FFT specific methods
		void FFTClear();
		void FFTPlan();
		
	private:
		std::string m_SkillsFile = "./data/skillsDB.txt";
		std::string m_ErrorMessage = ""; // this is shown to the user
		bool m_ErrorFlag = false; // this is used to control the program flow
		bool m_FFTCalculated = false;
		
		Img2FFTMode m_CurrentMode = Img2FFTMode::Idle;
		Img2FFTTasks m_CurrentTask = Img2FFTTasks::Idle;
		std::vector<Img2FFTTasks> m_TaskStack = {};
		std::vector<std::string> m_ColorSchemesNamesList = {};
		int m_ColorSchemeSelected = 0;
		std::string m_ColorSchemeSelectedName = "";
		std::unordered_map<std::string, std::vector<ImVec4>> m_ColorSchemesMap = {};
		std::unordered_map<std::string, Img2FFTColorScheme> m_ColorSchemes = {};
		FourierSpectrumMode m_FourierSpectrumMode = FourierSpectrumMode::Amplitude;
		
		float TEXT_BASE_WIDTH = 0.0f;
		float TEXT_BASE_HEIGHT = 0.0f;
		
		// fftw stuff
		bool m_fftw_planned = false;
		fftw_complex* m_fftw_in_width = nullptr;
		fftw_complex* m_fftw_out_width = nullptr;
		fftw_complex* m_fftw_in_height = nullptr;
		fftw_complex* m_fftw_out_height = nullptr;
		fftw_plan m_fftw_plan_width;
		fftw_plan m_fftw_plan_height;
		
		// media resources
		Image* m_Image = nullptr;
		
		// color features
		float lowerFFTWLevel = 0.0f;
		float upperFFTWlevel = 1.0f;
		
		void NewTask(Img2FFTTasks task);
		
		void ShowMainMenu();
		void ShowContent();
	};
}
