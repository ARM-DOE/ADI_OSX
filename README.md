#ADI

ARM Data Integrator, ADI, is an open source framework that automates the process of retrieving and preparing data for analysis, simplifies the design and creation of output data products produced by the analysis, and provides a modular, flexible software development architecture for implementing algorithms.  These capabilities are supported through the use of a workflow for data integration, a source code generator that produces C, IDL and Python templates, and a graphical interface through which users can efficiently define their data input, preprocessing, and output characteristics.  

ADI is used by the [Atmospheric Radiation Measurement (ARM) Climate Research Facility](http://www.arm.gov) to process raw data collected from ARM instruments, and to implement scientific algorithms that process one or more of ARM's existing data products to produce new, higher value, data products.

## To Install ADI:

- Install Homebrew if it's not already installed
- If you have installed a previous version execute the following
  - `brew uninstall adi`
  - `brew untap arm-doe/adi`
  - `brew tap arm-doe/adi`
  -  `brew tap homebrew/science`
  - `brew fetch --force adi`
  - `brew install adi`
- Otherwise for a first installation
  -  if installing on El Capitan (OSX 10.11.x) install openssl `brew install git --with-brewed-openssl`
  -  `brew tap homebrew/science`
  - `brew tap arm-doe/adi`
  - `brew install adi`
- For Python programmers:
  - First, ensure your default version of Python is python 2.7, and has numpy and Cython installed. The easiest way to do this for new users is to install [Anaconda](http://continuum.io/downloads).
  - If you have just installed Anaconda, be sure to restart your bash terminal session so that the new python version has been added to your path. Running `python --version` should show something like 
  
    `Python 2.7.10 :: Anaconda 2.3.0 (x86_64)`.
  - Download the adi python package from [here](https://engineering.arm.gov/~gaustad/adi-python-1.1.tar.gz)
  - untar the file, and from inside it, run:
  - `python setup.py install`
  - This should install the ADI libraries for whichever is your default version of python.

## Set Up an Environment for ADI

- Download [adi_home](https://engineering.arm.gov/~gaustad/adi_home.tar.gz). This tarfile contains a directory structure to get you started running ADI quickly. Note that you can also configure the directory structure however you like on your own. 
  
  *A bit of explanation here: ADI's VAPs expect certain directories and environment variables to be present, so to run VAPs, you
  must re-create these directories and environment variables. `adi_home` gets this started for you; it contains all the
  directories needed to run vaps, and provides the required environment variables in the file env_vars. You can use the example1
  vap to verify the ADI installation worked properly.*

- untar the file and copy the resulting `adi_home` directory to somewhere you want to work from, say, `~/Documents` or your home directory in /Users, i.e. `~`.  These instructions will assume you copied it into `~`.
- A core database named dsdb has been provided in /usr/local/share.  Create the directory path ~/adi_home/data/db/sqlite, `mkdir -p ~/adi_home/data/db/sqlite`
- Copy the core dsdb into it`cp /usr/local/share/dsdb/*core.sqlite   ~/adi_home/data/db/sqlite/dsdb.sqlite`
- Enter your home directory and create a new file named .db_connect with the following entries
  - dsdb_data    sqlite    ~/adi_home/data/db/sqlite/dsdb.sqlite
  - dsdb_read    sqlite    ~/adi_home/data/db/sqlite/dsdb.sqlite
- Enter the untarred `adi_home` directory
- To set the required environment variables for running from ~/adi_home, cat ~/adi_home/env_vars_bash, and copy all the commands into your bash terminal. Later you may want to add these environment variables to your .bash_profile, as THESE WILL HAVE TO BE SET EACH TIME YOU ENTER A NEW TERMINAL.  You will probably want to update ~/adi_home/env_vars_bash to set the locations explicitly so you can run your process from any location rather than from ~/adi_home.
- Setup the example vap process:
  - go into `dev/vap/src/adi_example1/process_dod_defs`
  - import the process by running `db_import_process -a dsdb_data -fmt json adi_example1.json`
  - import the output data definitions by running `db_load_dod -a dsdb_data cpc.json` and `db_load_dod -a dsdb_data met.json`

- Run C version of example1
  - if you have not edited ~/adi_home/env_vars_bash for your installtion, from the ~/adi_home directory 
  - run `make clean; make`. If successful the binary ~/adi_home/dev/vap/bin/adi_example1_vap will be created.
  - run `adi_example1_vap -s sbs -f S2 -b 20110401 -e 20110402 -D 2 -R` this should complete successfully with an exit status of zero.
  - The output data created are:
    ~/adi_home/data/datastream/sbs/sbsadicpcexample1S2.a1/sbsadicpcexample1S2.a1.20110401.000000.cdf
    ~/adi_home/data/datastream/sbs/sbsadimetexample1S2.a1/sbsadimetexample1S2.a1.20110401.000000.cdf

- Run Python version of example1
  - if you have not edited ~/adi_home/env_vars_bash for your installtion, from the ~/adi_home directory 
  - run `python adi_example1_vap.py -s sbs -f S2 -b 20110401 -e 20110402 -D 2 -R'
  - The output data created is the same as for the C run:
    ~/adi_home/data/datastream/sbs/sbsadicpcexample1S2.a1/sbsadicpcexample1S2.a1.20110401.000000.cdf
    ~/adi_home/data/datastream/sbs/sbsadimetexample1S2.a1/sbsadimetexample1S2.a1.20110401.000000.cdf

## To Add More Process Definitions to the DSDB:
The process definitions for adi_example1 have been included in your adi_home area. To run additional VAPs against your local database, you will need to import their process information.

- Get the process definition from the PCM
  - Go to the <a href="https://engineering.arm.gov/pcm/Main.html" target="_blank">Processing Configuration Manager</a>
    and select the processes tab on the left hand side
  - Type the name of the process you want in the filter at the bottom, or find it by scrolling through the list
  - Double click the name of the process to bring it up on the right hand side
  - Click *Text Export/Import* in the lower right corner, and copy the text that appears to a file on your machine
- Set your enviornment variables as specified in `env_vars_bash` from the last section 
- run `db_import_process` for the definition you retrieved
  - `db_import_process -a dsdb_data -fmt json <process definition file name>`
- Load the DODs nessecary to run this process. The DODs used by a process are listed on that process's page in the PCM.
  - Load the DOD into the PCM datastream viewer.
  - Select the JSON format from the green export DOD icon at the top of the page to copy the DOD to your clipboard. 
    Copy this into a file on your local machine
  - Load the dods into the local database
    - `db_load_dod -a dsdb_data <dod file>`

## Important Links

- [Documentation](http://engineering.arm.gov/ADI_doc)
- [ADI Process Configuration Interface](https://engineering.arm.gov/pcm/Main.html)
- [Examples](http://engineering.arm.gov/ADI_doc/algorithm.html#algorithm-development-tutorial)
- [ARM Data Archive for accessing ARM data](http://archive.arm.gov)
