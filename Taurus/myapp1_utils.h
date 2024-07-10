#include "hi_type.h"
#include "hi_comm_ive.h"
#include "hi_comm_video.h"

typedef struct HiSampleIveColorSpaceConvInfo {
    IVE_SRC_IMAGE_S stSrc;
    FILE* pFpSrc;
    FILE* pFpDst;
} SampleIveColorSpaceConvInfo;

// static HI_S32 SAMPLE_AUDIO_AiAenc(HI_VOID);

HI_S32 AUDIO_SAVE(HI_VOID);
HI_S32 AUDIO_INIT(HI_VOID);
HI_S32 AUDIO_START(HI_VOID);
HI_S32 AUDIO_STOP(HI_VOID);

HI_S32 AUDIO_READ_START_TAKEING_PHTOT(HI_VOID);
HI_S32 AUDIO_READ_START_START_RECORD(HI_VOID);
HI_S32 AUDIO_READ_START_END_RECORD(HI_VOID);
HI_S32 AUDIO_READ_START_AFTER_PHTOT(HI_VOID);

HI_S32 IMAGE_DISPLAY(HI_VOID);
HI_S32 IMAGE_CAPTURE(HI_VOID);

HI_S32 WIFI_CONNECT(HI_VOID);
HI_S32 NFS_START(HI_VOID);

int SWITCH_TEST(void);
int SWITCH_INIT(void);
int CHECK_SWITCH_F1(void);
int CHECK_SWITCH_F2(void);

HI_S32 SampleIveColorConvertInit(SampleIveColorSpaceConvInfo* g_stColorSpaceInfo, HI_CHAR* pchSrcFileName, HI_CHAR* pchDstFileName, HI_U32 u32Width, HI_U32 u32Height);
// int FrmToOrigImg(const VIDEO_FRAME_INFO_S* frm, IVE_IMAGE_S *img);
HI_VOID SampleIveColorConvertUninit(SampleIveColorSpaceConvInfo* pstColorConvertInfo);

int HISTREAMING_TEST(void);
int HISTREAMING_INIT(void);
int HISTREAMING_SEND(void);
int HISTREAMING_CLOSE(void);

HI_S32 DEMO_0(HI_VOID);
HI_S32 DEMO_1(HI_VOID);

int getState();
