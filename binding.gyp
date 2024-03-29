{
    "targets": [{
        "target_name": "filesystem-utilities",
        "sources": [ 
            'source/addon.cpp',
            'source/get_files_worker.cpp',
            'source/get_icon_worker.cpp',
            'source/get_exif_date_worker.cpp',
            'source/std_utils.cpp',
            'source/exif_reader.cpp'
        ],
        'include_dirs': [
            "<!@(node -p \"require('node-addon-api').include\")",
            "<!@(node -p \"var a = require('node-addon-api').include; var b = a.substr(0, a.length - 15); b + 'event-source-base' + a[a.length-1]\")"
        ],
        'dependencies': ["<!(node -p \"require('node-addon-api').gyp\")"],
        "cflags": ["-Wall", "-std=c++17"],
        'cflags_cc': ["-Wall", "-std=c++17"],
        'cflags!': [ '-fno-exceptions' ],
        'cflags_cc!': [ '-fno-exceptions' ],
        'link_settings': {
            "libraries": []
        },            
        'conditions': [
            ['OS=="win"', {
                'defines': [
                    'WINDOWS', 
                    '_SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING' 
                ],
                "msvs_settings": {
                    "VCCLCompilerTool": {
                        "ExceptionHandling": 1,
    					'AdditionalOptions': [
						    '-std:c++17',
						]
                    }
                },
                'sources': [
                    'source/windows/drives.cpp',
                    'source/windows/files.cpp',
                    'source/windows/icon.cpp',
                    'source/windows/file_version.cpp',
                    'source/windows/shell.cpp',
                    'source/windows/utils.cpp',
                    'source/windows/get_drives_worker.cpp',
                    'source/windows/get_file_version_worker.cpp',
                    'source/windows/create_directory_worker.cpp',
                    'source/windows/trash_worker.cpp',
                    'source/windows/copy_worker.cpp'
                ],
                "libraries": [ 
                    "gdiplus.lib",
                    "Mincore.lib"
                ]                
            }],
            ['OS=="linux"', {
                'defines': ['LINUX'],
                "libraries": ["<!@(pkg-config --libs gtk+-3.0)"],
                "cflags": [ "<!@(pkg-config --cflags gtk+-3.0)" ],
                'cflags_cc': [ "<!@(pkg-config --cflags gtk+-3.0)" ],
                "ldflags": ["<!@(pkg-config --libs gtk+-3.0)"],
                'link_settings': {
                    "libraries": [ 
                    ]
                },
                'sources': [ 
                    'source/linux/files.cpp',
                    'source/linux/icon.cpp',
                    'source/linux/copy_worker.cpp',
                    'source/linux/trash_worker.cpp'
                ]
            }],
        ]          
    }]
}
