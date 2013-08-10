#ifndef AirCppScriptCompiler_h__
#define AirCppScriptCompiler_h__

#include "AirCppScript.h"
#include <unordered_map>
#include <string>

namespace	Air{
	namespace	CppScript{
		class	Node;
		U32		WordToType(const char* str);
		U32		MakeType(enumWordType w,U32 main,U32 sub,U32 flag);
		class	Compiler{
		public:
			Compiler();
			virtual	~Compiler();

			bool	Initialization();
			bool	Release();

			bool	Compile(const void* pBuffer,unsigned int uiSize);
			bool	Compile(const wchar_t* pName);
		protected:
			void	AllocWordInfo();
			void	FreeWordInfo();
			U32		WorldToWordInfo(StringVector& vecWord,WordInfoVector& vecInfo);
			U32		WordInfoToSyntaxTree(WordInfoVector& vecInfo);
		protected:
			AChar*		m_pWordBuffer;
			WordInfo*	m_pWordInfo;
			U32			m_uiWordCount;
			std::tr1::unordered_map<std::string,U32>	m_mapWordType;

		};
	}
}
#endif // AirCppScriptCompiler_h__