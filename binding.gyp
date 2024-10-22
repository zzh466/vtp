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
        "<(module_root_dir)/20220613_tradeapi64_se_windows/WinDataCollect.lib",
        "<(module_root_dir)/20220613_tradeapi64_se_windows/thosttraderapi_se.lib",
        "<(module_root_dir)/20220613_tradeapi64_se_windows/thostmduserapi_se.lib"
      ],
      "include_dirs": ["20220613_tradeapi64_se_windows/","<!(node -e \"require('nan')\")"],
      "copies": [
              {
                "destination": "<(module_root_dir)/build/Release/",
                "files": [
                  "<(module_root_dir)/20220613_tradeapi64_se_windows/thosttraderapi_se.dll",
                  "<(module_root_dir)/20220613_tradeapi64_se_windows/thostmduserapi_se.dll",
                  "<(module_root_dir)/20220613_tradeapi64_se_windows/WinDataCollect.dll"
                ]
              },
              {
                "destination": "<(module_root_dir)/build/Debug/",
                "files": [
                  "<(module_root_dir)/20220613_tradeapi64_se_windows/thosttraderapi_se.dll",
                  "<(module_root_dir)/20220613_tradeapi64_se_windows/thostmduserapi_se.dll",
                  "<(module_root_dir)/20220613_tradeapi64_se_windows/WinDataCollect.dll"
                ]
              }
            ]
    }
  ]
}
