{
    "targets": [{
        "target_name": "filesystem-utilities",
        "sources": [ 
            'source/addon.cpp',
            'source/nullfunction.cpp',
            'source/get_drives_worker.cpp',
            'source/std_utils.cpp'
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
                    'source/windows/drives.cpp'
                ]                
            }],
            ['OS=="linux"', {
                'defines': ['LINUX'],
                'libraries!': [ ],
                'sources': [ ]
            }],
        ]          
    }]
}
