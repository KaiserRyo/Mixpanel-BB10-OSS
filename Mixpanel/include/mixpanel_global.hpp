/*
 * Copyright (c) 2011-2014 BlackBerry Limited.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef Mixpanel_Global_HPP_
#define Mixpanel_Global_HPP_

#include <QtCore/qglobal.h>

#ifndef MIXPANEL_STATIC_LINK
  #ifdef MIXPANEL_LIBRARY
    #define MIXPANEL_EXPORT Q_DECL_EXPORT
  #else
    #define MIXPANEL_EXPORT Q_DECL_IMPORT
  #endif
#else
  #define MIXPANEL_EXPORT
#endif /* MIXPANEL_STATIC_LINK */

#endif /* Mixpanel_Global_HPP_ */
