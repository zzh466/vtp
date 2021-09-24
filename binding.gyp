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
      "include_dirs": ["ctp_20210406tradeapi_v6.6.1/"],
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
  ]
}
