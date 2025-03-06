{
  "targets": [
    {
      "target_name": "ctp",
      "sources": [
        ".\\src\\ctp\\cpp\\ctp.cc",
        ".\\src\\ctp\\cpp\\wrap_trader.cpp",
        ".\\src\\ctp\\cpp\\uv_trader.cpp"
      ],
      "libraries": [
        "<(module_root_dir)/ctp_20210406tradeapi_v6.6.1/WinDataCollect.lib",
        "<(module_root_dir)/ctp_20210406tradeapi_v6.6.1/thosttraderapi_se.lib",
        "<(module_root_dir)/ctp_20210406tradeapi_v6.6.1/thostmduserapi_se.lib"
      ],
      "include_dirs": ["ctp_20210406tradeapi_v6.6.1/","<!(node -e \"require('nan')\")"],
      "copies": [
              {
                "destination": "<(module_root_dir)/build/Release/",
                "files": [
                  "<(module_root_dir)/ctp_20210406tradeapi_v6.6.1/thosttraderapi_se.dll",
                  "<(module_root_dir)/ctp_20210406tradeapi_v6.6.1/thostmduserapi_se.dll",
                  "<(module_root_dir)/ctp_20210406tradeapi_v6.6.1/WinDataCollect.dll"
                ]
              },
              {
                "destination": "<(module_root_dir)/build/Debug/",
                "files": [
                  "<(module_root_dir)/ctp_20210406tradeapi_v6.6.1/thosttraderapi_se.dll",
                  "<(module_root_dir)/ctp_20210406tradeapi_v6.6.1/thostmduserapi_se.dll",
                  "<(module_root_dir)/ctp_20210406tradeapi_v6.6.1/WinDataCollect.dll"
                ]
              }
            ]
    }
    ,
    {
      "target_name": "qdp",
      "sources": [
        ".\\src\\qdp\\cpp\\qdp.cc",
        ".\\src\\qdp\\cpp\\wrap_trader_qdp.cpp",
        ".\\src\\qdp\\cpp\\uv_trader_qdp.cpp"
      ],
      "libraries": [
        "<(module_root_dir)/qdp5.0.1_api_win_20230817/qdptraderapi.lib"
      ],
      "include_dirs": ["qdp5.0.1_api_win_20230817/","<!(node -e \"require('nan')\")"],
      "copies": [
              {
                "destination": "<(module_root_dir)/build/Release/",
                "files": [
                  "<(module_root_dir)/qdp5.0.1_api_win_20230817/qdptraderapi.dll"
                ]
              },
              {
                "destination": "<(module_root_dir)/build/Debug/",
                "files": [
                  "<(module_root_dir)/qdp5.0.1_api_win_20230817/qdptraderapi.dll"
                ]
              }
            ]
    }
  ]
}