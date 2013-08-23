#include "AirCppScriptModule.h"
#include "AirCppScript.h"
#include <Windows.h>
#include <time.h>
#include "AirCppScriptFunction.h"

#define CPP_SCRIPT_MASK		'SppC'
#define COMPILE_VERSION		'10VC'
#define FILE_VERSION_01		'10VF'

void	PrintTest(const char* str){
	printf(str);
	printf("\n");
	
}

namespace	Air{
	namespace	CppScript{
		struct InjectCppScriptFunction{
			AString		strName;
			void*		pFunction;
		};
		InjectCppScriptFunction	InjectFunc[]={
			{"malloc",__Alloc},
			{"free",__Free},
			{"Print",PrintTest}
		};

		Module::Module()
		{
			m_pImageBase	=	NULL;
			m_uiSize		=	0;
			m_pHeader		=	NULL;
		}
		Module::~Module()
		{
			UnLoad();
		}
		enumLoadError Module::Load( const wchar_t* strName )
		{
			void*	pBuffer		=	NULL;
			U32		uiSize		=	0;
			if(!LoadFile(strName,pBuffer,uiSize)){
				return enLE_File_Isnt_Exist;
			}
			enumLoadError	e	=	Load(pBuffer,uiSize);
			if(e!=enLE_OK){
				__Free(pBuffer);
				return e;
			};
			return e;
		}

		enumLoadError Module::Load( const void* pBuffer,U32 uiSize )
		{
			
			if(uiSize	<	sizeof(ModuleHeader)){
				return enLE_File_Size_Less_Than_Module_Header;
			}
			m_pHeader	=	(ModuleHeader*)pBuffer;
			if(m_pHeader->CppS!=CPP_SCRIPT_MASK){
				return enLE_Unknown_File_Format;
			}
			m_uiSize		=	uiSize;
			m_pImageBase	=	VirtualAlloc(0,m_uiSize,MEM_COMMIT,PAGE_EXECUTE_READWRITE);
			memcpy(m_pImageBase,pBuffer,m_uiSize);
			m_pHeader		=	(ModuleHeader*)m_pImageBase;

			if(m_pHeader->Version==0){
				return LoadRelocaltion_VF01();
			}else{
				UnLoad();
				return enLE_Unknown_Version;
			}
			return enLE_OK;
		}

		enumLoadError Module::LoadRelocaltion_VF01()
		{
			//Entry
			if(m_pHeader->Entry_RA!=0){
				m_pHeader->Entry_RA	+=	(U32)m_pImageBase;
			}

			//IAT
			m_pHeader->IAT_RA			+=	(U32)m_pImageBase;
			m_pHeader->IAT_Name_RA		+=	(U32)m_pImageBase;
			U32*	pName	=	(U32*)m_pHeader->IAT_Name_A;
			
			for(U32 i=0;i<m_pHeader->IATCount;i++){
				pName[i]	+=	(U32)m_pImageBase;
				
				//Find Import Function Address
				void*	pRealAddr	=	NULL;
				for(U32 j=0;j<3;j++){
					if(strcmp(InjectFunc[j].strName.c_str(),m_pHeader->IAT_Name_A[i])==0){
						pRealAddr	=	InjectFunc[j].pFunction;
						break;
					}
				}
				if(pRealAddr==NULL){
					return enLE_Cant_Find_Import_Function;
				}

				//Relocaltion
				U8* pIAT	=	m_pHeader->IAT_A;
				pIAT+=	5*i	+ 1;
				U32	uiCurrent	=	(U32)pIAT + 4;
				U32	uiOffset	=	(U32)pRealAddr	-	uiCurrent;
				memcpy(pIAT,&uiOffset,sizeof(U32));
			}

			//EAT
			m_pHeader->EAT_RA			+=	(U32)m_pImageBase;
			m_pHeader->EAT_Name_RA		+=	(U32)m_pImageBase;
			pName				=	(U32*)m_pHeader->EAT_Name_A;
			U32*	pExportFunc	=	(U32*)m_pHeader->EAT_RA;
			for(U32 i=0;i<m_pHeader->EATCount;i++){
				pName[i]		+=	(U32)m_pImageBase;
				pExportFunc[i]	+=	(U32)m_pImageBase;
			}
			return enLE_OK;
		}

		void Module::UnLoad()
		{
			if(m_pImageBase!=NULL){
				VirtualFree(m_pImageBase,0,MEM_FREE);
				m_pImageBase=NULL;
			}
			m_uiSize=0;
			m_pHeader=NULL;
		}

		void* Module::FindFunction( const char* strFunctionName )
		{
			for(U32 i=0;i<m_pHeader->EATCount;i++){
				if(strcmp(m_pHeader->EAT_Name_A[i],strFunctionName)==0){
					return m_pHeader->EAT_A[i];
				}
			}
			return NULL;
		}

		void InitModuleHeader( ModuleHeader& header )
		{
			header.CppS				=	CPP_SCRIPT_MASK;
			header.CompilerVersion	=	COMPILE_VERSION;
			header.Version			=	FILE_VERSION_01;
			time_t timep;
			time(&timep); 
			header.TimeStamp		=	timep;
			CoCreateGuid(&header.Guid);
			header.Age				=	1;
			header.IATCount			=	0;
			header.IAT_RA			=	0;
			header.IAT_Name_RA		=	0;
			header.EATCount			=	0;
			header.EAT_RA			=	0;
			header.EAT_Name_RA		=	0;
			header.Entry_RA			=	0;
			header.ImageSize		=	0;
			header.Code[0]			=	0x90;
			header.Code[0]			=	0x90;
			header.Code[0]			=	0x90;
			header.Code[0]			=	0x90;
		}



	}
}