#ifndef QUtauGlobalH
#define QUtauGlobalH

/*===========================================================================
    UTAU Sequence Text - 一些枚举类型
    C++ Version : Copyright (C) 2010-2011 by SHINTA
    Qt Version : Copyright (C) 2020-2021 by SineStriker
===========================================================================*/

enum {
    MFM_OMITTED,
    MFM_ORIGINAL,
    MFM_ORIGIN_REMOVED,
    MFM_MODIFIED,
    MFM_MODIFY_REMOVED,
    MFM_NEW,
    MFM_NEW_REMOVED
};

enum {
    ERROR_INVALID_INPUT_FILE_PATH,      // 输入文件路径非法
    ERROR_INVALID_FORMAT,               // 输入文件格式不正确
    MINOR_ERROR,                        // # 分隔不能打开与能打开
    ERROR_INVALID_VOICE_DIR_PATH,       // 音源路径非法
    ERROR_NO_SETTING_INI,               // 没有setting.ini
    ERROR_NO_TOOLS_IN_SETTING_INI,      // setting.ini中没有工具
    ERROR_INVALID_TOOLS_IN_SETTING_INI, // setting.ini中的工具路径非法
    ERROR_INVALID_TOOLS_IN_PROJECT_UST, // project中的工具路径非法
    NONE_ERROR                          // # 没有错误
};
// 省略的，自定义的，管道文件中存在的，修改的，删除的，新增的（不知道是否是自定义键）

#endif
