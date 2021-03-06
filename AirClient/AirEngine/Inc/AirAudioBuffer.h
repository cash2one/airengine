#ifndef AUDIOBUFFER_HEAD_FILE
#define AUDIOBUFFER_HEAD_FILE

#include "AirEngineHeader.h"
namespace Air{
	
	namespace Engine{
	
		namespace	Audio{
			enum enumFormat{
				enF_MONO8		=	0x1100,
				enF_MONO16		=	0x1101,
				enF_STEREO8		=	0x1102,
				enF_STEREO16	=	0x1103
			};
			/**	\brief	声音缓冲区
			*
			*	声音缓冲区
			*
			***/
			class	ENGINE_EXPORT	Buffer	:
				public	IProduct{
			public:
				static AString	ProductTypeName;
				Buffer(CAString&	strName);
	
				/**	\brief	创建
				*   
				*	@remarks 	创建
				*	@see		Buffer
				*	@return   	U1
				*	@note
				*
				**/
				virtual	U1	Create();
				/**	\brief	摧毁
				*   
				*	@remarks 	摧毁
				*	@see		Buffer
				*	@return   	U1
				*	@note
				*
				**/
				virtual	U1	Destroy();
	
				/**	\brief	获取缓冲区
				*   
				*	@remarks 	获取缓冲区
				*	@see		Buffer
				*	@return   	U8*
				*	@note
				*
				**/
				U8*		GetBuffer(){
					return	m_pBuffer;
				};
				/**	\brief	获取缓冲区大小
				*   
				*	@remarks 	获取缓冲区大小
				*	@see		Buffer
				*	@return   	UInt
				*	@note
				*
				**/
				UInt	GetBufferSize(){
					return	m_uiSize;
				};
	
				/**	\brief	获取音频格式
				*   
				*	@remarks 	获取音频格式
				*	@see		Buffer
				*	@return   	SInt
				*	@note
				*
				**/
				enumFormat	GetFormat(){
					return	m_uiFormat;
				};
				/**	\brief	获取序列
				*   
				*	@remarks 	获取序列
				*	@see		Buffer
				*	@return   	SInt
				*	@note
				*
				**/
				SInt	GetFreqBuffer(){
					return	m_uiFreqBuffer;
				}
	
			protected:
				/**	\brief	载入WAV格式文件
				*   
				*	@remarks 	载入WAV格式文件
				*	@see		Buffer
				*	@return   	U1
				*	@param		Data * pData
				*	@note
				*
				**/
				U1		LoadWAVFromMemory(Data*	pData);
			protected:
				U8*			m_pBuffer;			///<	音频缓冲
				UInt		m_uiSize;			///<	音频缓冲大小
				enumFormat	m_uiFormat;			///<	音频格式
				SInt		m_uiFreqBuffer;		///<	序列
			};
		}
	
	};
};
#endif // AUDIOBUFFER_HEAD_FILE