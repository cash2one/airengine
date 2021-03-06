#ifndef MClientFile_h__
#define MClientFile_h__

#ifndef MCLIENT_EXPORT
#ifdef MCLIENT_IMPORT
#define MCLIENT_EXPORT __declspec(dllexport)
#else
#define MCLIENT_EXPORT __declspec(dllimport)
#endif
#endif

#define	MFILE_EXIST		(void*)0
#define	MFILE_READY		(void*)1
#define	MFILE_DOWNLOAD	(void*)2


extern "C"	MCLIENT_EXPORT	bool	DllInit();
extern "C"	MCLIENT_EXPORT	bool	DllRelease();

extern "C"	MCLIENT_EXPORT	unsigned int	LoadFile(const char* strName,void* pBuffer);

#endif // MClientFile_h__
