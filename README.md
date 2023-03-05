# ccruncher

CCruncher is a project for quantifying portfolio credit risk using the copula approach. CCruncher evaluates the portfolio credit risk by sampling the portfolio loss distribution and computing the Expected Loss (EL), Value at Risk (VaR) and Expected Shortfall (ES) statistics. The portfolio losses are obtained simulating the default times of obligors and simulating the EADs and LGDs of their assets.

This repository contains the elements to create :

* __Technical document__ : A [pdf document](https://www.ccruncher.net/ccruncher.pdf) describing the mathematical model, parameter estimation, risk assessment and portfolio optimization.
* __ccruncher_inf__ : Program to infer the model parameters from historical data.
* __ccruncher_cmd__: Program to compute the portfolio risk (command line).
* __ccruncher_gui__: Program to compute the portfolio risk (gui).
* __Site__: HTML site files describing the input file format, FAQs, install, etc.

The project has reached a stable, usable state but is no longer being actively developed; support/maintenance will be provided as time allows.

For further information you can refer to the [ccruncher site](https://ccruncher.net) or contact the [autor](mailto:gtorrent@ccruncher.net).

## Authors

* __Gerard Torrent__ - *Initial work* - [torrentg](https://github.com/torrentg/)

## License

This project is licensed under the GNU GPL v2 License - see the [COPYING](doc/COPYING) file for details.
