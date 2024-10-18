# Options Killer Bot CPP

This is a C++ bot for options trading that is meant to speed up performance over the python implementation. The application includes custom interpolation models to analyze option chains and mispricings.

## Requirements

Make sure you have the following dependencies installed:

- Eigen
- Curl
- nlohmann/json
- CMake 3.14 or later

You can install these libraries via vcpkg or your preferred package manager.

## Configuration

1. Create a `.env` file in the root directory with the following structure:
 ```env
    SCHWAB_API_KEY=your_schwab_api_key 
    SCHWAB_SECRET=your_schwab_secret 
    SCHWAB_CALLBACK_URL=your_callback_url 
    SCHWAB_ACCOUNT_HASH=your_account_hash 
    FRED_API_KEY=your_fred_api_key 
    DRY_RUN=true 
    TIME_TO_REST=2
```

2. Create a `stocks.json` file in the root directory with the following structure:
 ```json
[   
    { 
        "ticker": "JPM", 
        "date_index": 0, 
        "option_type": "calls", 
        "min_overpriced": 0.14, 
        "min_oi": 400.0 
    } 
]
```

## Usage

1. Clone the repository and navigate to the project folder:

`git clone https://github.com/hedge0/OptionsKillerBotCPP.git cd OptionsKillerBotCPP`


2. Build the project using CMake:

`mkdir build cd build cmake .. make`

3. Run the bot using the following command:

`./OptionsKillerBotCPP`

## Features

- **Option Chain Filtering**: Filters option chains based on bid price, implied volatility, and open interest.
- **Model Fitting**: Fits various models (RBF, RFV) to the implied volatility data to find the best fit for pricing.
- **CSV Output**: The bot outputs original and interpolated IV data to CSV for analysis.

## License

All Rights Reserved License (ARR).

This project is licensed under an All Rights Reserved License (ARR). See the LICENSE file for more details.
