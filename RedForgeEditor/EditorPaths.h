#pragma once

#include "FileManager.h"

#define QICON_FROM_PATH(iconLocalPath) QIcon(QString(GetEditorAssetsPath().append("Icons").append(iconLocalPath).string().c_str()))