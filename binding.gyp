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
    },
    {
      "target_name": "pet",
      "sources": [
        ".\\src\\pet\\pet\\pet.cc",
        ".\\src\\pet\\pet\\uv_wrap_trader_pet.cpp",
        ".\\src\\pet\\pet\\uv_trader_pet.cpp"
      ],
      "libraries": [
        "<(module_root_dir)/pet_v1.0.3/IcfWrapper.lib"
      ],
      "include_dirs": ["pet_v1.0.3/","<!(node -e \"require('nan')\")"],
      "copies": [
        {
          "destination": "<(module_root_dir)/build/Release/",
          "files": [
            "<(module_root_dir)/pet_v1.0.3/IcfWrapper.dll"
          ]
        },
        {
          "destination": "<(module_root_dir)/build/Debug/",
          "files": [
            "<(module_root_dir)/pet_v1.0.3/IcfWrapper.dll"
          ]
        }
      ]
    }
  ]
}
