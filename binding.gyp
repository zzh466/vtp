{
  "targets": [
    {
      "target_name": "ctp",
      "product_dir": "<(module_root_dir)/build/Release/", 
      "sources": [
        ".\\src\\ctp\\cpp\\ctp.cc",
        ".\\src\\ctp\\cpp\\wrap_trader.cpp",
        ".\\src\\ctp\\cpp\\uv_trader.cpp"
      ],
      "libraries": [
        # "<(module_root_dir)/20220613_tradeapi64_se_windows/WinDataCollect.lib",
        "<(module_root_dir)/20250415_traderapi64_se_windows/thosttraderapi_se.lib",
        "<(module_root_dir)/20250415_traderapi64_se_windows/thostmduserapi_se.lib"
      ],
      "include_dirs": ["20250415_traderapi64_se_windows/","<!(node -e \"require('nan')\")"],
      "copies": [
              {
                "destination": "<(module_root_dir)/build/Release/",
                "files": [
                  "<(module_root_dir)/20250415_traderapi64_se_windows/thosttraderapi_se.dll",
                  "<(module_root_dir)/20250415_traderapi64_se_windows/thostmduserapi_se.dll",
                  # "<(module_root_dir)/20220613_tradeapi64_se_windows/WinDataCollect.dll"
                ]
              },
              {
                "destination": "<(module_root_dir)/build/Debug/",
                "files": [
                  "<(module_root_dir)/20250415_traderapi64_se_windows/thosttraderapi_se.dll",
                  "<(module_root_dir)/20250415_traderapi64_se_windows/thostmduserapi_se.dll",
                  # "<(module_root_dir)/20220613_tradeapi64_se_windows/WinDataCollect.dll"
                ]
              }
            ]
    },
    # {
    #   "target_name": "qdp",
    #   "product_dir": "<(module_root_dir)/build/Release/QDP/", 
    #   "sources": [
    #     ".\\src\\qdp\\cpp\\qdp.cc",
    #     ".\\src\\qdp\\cpp\\wrap_trader_qdp.cpp",
    #     ".\\src\\qdp\\cpp\\uv_trader_qdp.cpp"
    #   ],
    #   "libraries": [
    #     "<(module_root_dir)/qdp5.0.1_api_win_20230817/qdptraderapi.lib"
    #   ],
    #   "include_dirs": ["qdp5.0.1_api_win_20230817/","<!(node -e \"require('nan')\")"],
    #   "copies": [
    #           {
    #             "destination": "<(module_root_dir)/build/Release/QDP/",
    #             "files": [
    #               "<(module_root_dir)/qdp5.0.1_api_win_20230817/qdptraderapi.dll"
    #             ]
    #           },
    #           {
    #             "destination": "<(module_root_dir)/build/Debug/QDP/",
    #             "files": [
    #               "<(module_root_dir)/qdp5.0.1_api_win_20230817/qdptraderapi.dll"
    #             ]
    #           }
    #         ]
    # },
    # {
    #   "target_name": "ctpmini",
    #   "product_dir": "<(module_root_dir)/build/Release/", 
    #   "sources": [
    #     ".\\src\\ctpmini\\cpp\\ctpmini.cc",
    #     ".\\src\\ctpmini\\cpp\\wrap_trader.cpp",
    #     ".\\src\\ctpmini\\cpp\\uv_trader.cpp"
    #   ],
    #   "libraries": [
    #     # "<(module_root_dir)/ctpmini_20240923/WinDataCollect.lib",
    #     "<(module_root_dir)/ctpmini_20240923/thosttraderapi.lib",
    #     "<(module_root_dir)/ctpmini_20240923/thostmduserapi.lib"
    #   ],
    #   "include_dirs": ["ctpmini_20240923/","<!(node -e \"require('nan')\")"],
    #   "copies": [
    #           {
    #             "destination": "<(module_root_dir)/build/Release/",
    #             "files": [
    #               "<(module_root_dir)/ctpmini_20240923/thosttraderapi.dll",
    #               "<(module_root_dir)/ctpmini_20240923/thostmduserapi.dll",
    #               # "<(module_root_dir)/ctpmini_20240923/WinDataCollect.dll"
    #             ]
    #           },
    #           {
    #             "destination": "<(module_root_dir)/build/Debug/",
    #             "files": [
    #               "<(module_root_dir)/ctpmini_20240923/thosttraderapi.dll",
    #               "<(module_root_dir)/ctpmini_20240923/thostmduserapi.dll",
    #               # "<(module_root_dir)/ctpmini_20240923/WinDataCollect.dll"
    #             ]
    #           }
    #         ]
     # }
    # ,
    {
      "target_name": "ctpmd",
      "product_dir": "<(module_root_dir)/build/Release/", 
      "sources": [
        ".\\src\\ctpmd\\cpp\\ctpmd.cc",
        ".\\src\\ctpmd\\cpp\\wrap_mduser.cpp",
        ".\\src\\ctpmd\\cpp\\uv_mduser.cpp"
      ],
      "libraries": [
        # "<(module_root_dir)/20220613_tradeapi64_se_windows/WinDataCollect.lib",
        "<(module_root_dir)/20250415_traderapi64_se_windows/thosttraderapi_se.lib",
        "<(module_root_dir)/20250415_traderapi64_se_windows/thostmduserapi_se.lib"
      ],
      "include_dirs": ["20250415_traderapi64_se_windows/","<!(node -e \"require('nan')\")"],
      "copies": [
              {
                "destination": "<(module_root_dir)/build/Release/",
                "files": [
                  "<(module_root_dir)/20250415_traderapi64_se_windows/thosttraderapi_se.dll",
                  "<(module_root_dir)/20250415_traderapi64_se_windows/thostmduserapi_se.dll",
                  # "<(module_root_dir)/20220613_tradeapi64_se_windows/WinDataCollect.dll"
                ]
              },
              {
                "destination": "<(module_root_dir)/build/Debug/",
                "files": [
                  "<(module_root_dir)/20250415_traderapi64_se_windows/thosttraderapi_se.dll",
                  "<(module_root_dir)/20250415_traderapi64_se_windows/thostmduserapi_se.dll",
                  # "<(module_root_dir)/20220613_tradeapi64_se_windows/WinDataCollect.dll"
                ]
              }
            ]
    }
    # {
    #   "target_name": "qdp",
    #   "sources": [
    #     ".\\src\\qdp\\cpp\\qdp.cc",
    #     ".\\src\\qdp\\cpp\\wrap_trader_qdp.cpp",
    #     ".\\src\\qdp\\cpp\\uv_trader_qdp.cpp"
    #   ],
    #   "libraries": [
    #     "<(module_root_dir)/qdp5.0.1_api_win_20230817/qdptraderapi.lib"
    #   ],
    #   "include_dirs": ["qdp5.0.1_api_win_20230817/","<!(node -e \"require('nan')\")"],
    #   "copies": [
    #           {
    #             "destination": "<(module_root_dir)/build/Release/",
    #             "files": [
    #               "<(module_root_dir)/qdp5.0.1_api_win_20230817/qdptraderapi.dll"
    #             ]
    #           },
    #           {
    #             "destination": "<(module_root_dir)/build/Debug/",
    #             "files": [
    #               "<(module_root_dir)/qdp5.0.1_api_win_20230817/qdptraderapi.dll"
    #             ]
    #           }
    #         ]
    # }
  ]
}