/*
 * Copyright (c) 2022 HiSilicon (Shanghai) Technologies CO., LIMITED.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef SMP_COLOR_SPACE_CONVERT_H
#define SMP_COLOR_SPACE_CONVERT_H

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

void SampleIveOrigImgToFrm(void);
void SampleIveFrmToOrigImg(void);
void SampleIveFrmToU8c1Img(void);
void SampleIveFrmToRgbImgToYuv(void);
void SampleIveFrmToRgbImgToBgr(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* End of #ifndef SMP_COLOR_SPACE_CONVERT_H */
