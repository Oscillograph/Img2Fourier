#ifndef Img2FFT_LOGGER_H
#define Img2FFT_LOGGER_H

#include <include/common.h>
#include <external/utf8.h>

// comment this line if you want something simple like std::cout
#define Img2FFT_LOGGER_CLASS

typedef std::basic_stringstream<char32_t> u32stringstream;

class Logger
{
public:
	static void Flush(int mode = 0, int color = -1);
	
	template<typename... Args>
	inline static void Text(Args... args)
	{
		m_u8stream.str(std::string()); // clear utf8 buffer
		m_u32string = U""; // clear utf32 string
		
		// add args to utf8 string stream
		m_u8stream << "[" << Utils::GetDateAndTime() << "] ";
		(u8add(std::forward<Args>(args)), ...);
		m_u8stream << "\n";
		m_u8string = m_u8stream.str();
		
		// convert it to utf32
		utf8::utf8to32(m_u8string.begin(), m_u8string.end(), std::back_inserter(m_u32string));
		
		m_u32stream << m_u32string << "\n";
	}
	
	template<typename... Args>
	inline static void Message(Args... args)
	{
		m_u8stream.str(std::string()); // clear utf8 buffer
		m_u32string = U""; // clear utf32 string
		
		// add args to utf8 string stream
		// m_u8stream << "[" << Utils::GetDateAndTime() << "] ";
		(u8add(std::forward<Args>(args)), ...);
		m_u8stream << "\n";
		m_u8string = m_u8stream.str();
		
		// convert it to utf32
		utf8::utf8to32(m_u8string.begin(), m_u8string.end(), std::back_inserter(m_u32string));
		
		m_u32stream << m_u32string << "\n";
	}
	
private:
	inline static void Reset()
	{
		m_u8string = "";
		m_u32string = U"";
		m_u8stream.str(std::string());
		m_u32stream.str(std::u32string());
	}
	
	template <typename T>
	inline static void u8add(T t) { m_u8stream << t; }
	
	template <typename T>
	inline static void u32add(T t) { m_u32stream << t; }
	
	static std::stringstream m_u8stream;
	static std::string m_u8string;
	static std::u32string m_u32string;
	static u32stringstream m_u32stream;
};

#ifndef Img2FFT_LOGGER_CLASS
#define CONSOLE_LOG(...)			std::cout << "SkillsChecker: " << __VA_ARGS__ << "\n";
#define CONSOLE_DEBUG(...)			std::cout << "SkillsChecker: " << __VA_ARGS__ << "\n";
#define CONSOLE_ERROR(...)			std::cout << "SkillsChecker: " << __VA_ARGS__ << "\n";
#define CONSOLE_ASSERT(x, ...)		{ if (!(x)) { SkillsChecker_CORE_LOG(__VA_ARGS__); std::exit(); } }

#define CONSOLE_LOG(...) 				std::cout << "App: " << __VA_ARGS__ << "\n";
#define CONSOLE_DEBUG(...)				std::cout << "App: " << __VA_ARGS__ << "\n";
#define CONSOLE_ERROR(...)				std::cout << "App: " << __VA_ARGS__ << "\n";
#define CONSOLE_ASSERT(x, ...)			{ if (!(x)) { SkillsChecker_LOG(__VA_ARGS__); std::exit(); } }
#else
#define CONSOLE_LOG(...)			::Logger::Text("Img2FFT: ", __VA_ARGS__); ::Logger::Flush(0, 0);
#define CONSOLE_ERROR(...)			::Logger::Text("Img2FFT: ", __VA_ARGS__); ::Logger::Flush(0, 3);
#define CONSOLE_ASSERT(x, ...)		{ if (!(x)) { SkillsChecker_CORE_LOG(__VA_ARGS__); std::exit(-4); } }
#define CONSOLE_DEBUG(...)			::Logger::Text("Img2FFT: ", __VA_ARGS__); ::Logger::Flush(1, 0);

#define CONSOLE_GREY(...)			::Logger::Message(__VA_ARGS__); ::Logger::Flush(0, -1);
#define CONSOLE_YELLOW(...)			::Logger::Message(__VA_ARGS__); ::Logger::Flush(0, 0);
#define CONSOLE_WHITE(...)			::Logger::Message(__VA_ARGS__); ::Logger::Flush(0, 1);
#define CONSOLE_DARKRED(...)		::Logger::Message(__VA_ARGS__); ::Logger::Flush(0, 2);
#define CONSOLE_RED(...)			::Logger::Message(__VA_ARGS__); ::Logger::Flush(0, 3);
#define CONSOLE_TEAL(...)			::Logger::Message(__VA_ARGS__); ::Logger::Flush(0, 4);
#define CONSOLE_CYAN(...)			::Logger::Message(__VA_ARGS__); ::Logger::Flush(0, 5);
#define CONSOLE_BLUE(...)			::Logger::Message(__VA_ARGS__); ::Logger::Flush(0, 6);
#define CONSOLE_MAGENTA(...)		::Logger::Message(__VA_ARGS__); ::Logger::Flush(0, 7);
#define CONSOLE_GREEN(...)			::Logger::Message(__VA_ARGS__); ::Logger::Flush(0, 8);
#define CONSOLE_CAPTION_BLACK(...)	::Logger::Message(__VA_ARGS__); ::Logger::Flush(0, 9);

//#define CONSOLE_LOG(...)				::Logger::Text("App: ", __VA_ARGS__); ::Logger::Flush(0, 1);
//#define CONSOLE_ERROR(...)			::Logger::Text("App: ", __VA_ARGS__); ::Logger::Flush(0, 3);
//#define CONSOLE_DEBUG(...)			::Logger::Text("App: ", __VA_ARGS__); ::Logger::Flush(1, 1);
//#define CONSOLE_ASSERT(x, ...)			{ if (!(x)) { SkillsChecker_LOG(__VA_ARGS__); std::exit(-4); } }
#endif

#endif

