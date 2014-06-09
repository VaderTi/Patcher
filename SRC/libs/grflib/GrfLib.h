#pragma once

#define GRFEXPORT extern

#ifdef __cplusplus
extern "C" {
#endif

class CGrf;
class CFileNode;

typedef CGrf*		GrfFile;
typedef CFileNode*	GrfNode;

enum GRF_FLAGS
{
	GRF_FLAG_FILE		= 1,
	GRF_FLAG_MIXCRYPT	= 2,
	GRF_FLAG_DES		= 4,
	GRF_FLAG_DELETED	= 8
};

enum ERROR_FLAGS
{
	GRF_OK = 0
};

typedef bool(*WORKPROGRESS)(const char *FileName, unsigned Current, unsigned Total, void *pData);

GRFEXPORT GrfFile		GrfCreate(const char *FileName);
GRFEXPORT GrfFile		GrfOpen(const char *FileName, bool AutoCreate = false);
GRFEXPORT bool			GrfMerge(GrfFile Grf, const char *SrcGrfName);
GRFEXPORT bool			GrfRepack(GrfFile Grf);
GRFEXPORT void			GrfDelete(GrfFile Grf, const char* FileName);
GRFEXPORT bool			GrfSave(GrfFile Grf);
GRFEXPORT void			GrfSetCallback(GrfFile Grf, WORKPROGRESS OnProgress = nullptr, void *pData = nullptr);
GRFEXPORT void			GrfClose(GrfFile Grf);
GRFEXPORT void			GrfFree(GrfFile Grf);

GRFEXPORT const char*	GrfLastError(GrfFile Grf);

GRFEXPORT void ExtractRGZ(const char *RgzName);

#ifdef __cplusplus
}
#endif