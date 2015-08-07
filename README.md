#ADI

ARM Data Integrator, ADI, is an open source framework that automates the process of retrieving and preparing data for analysis, simplifies the design and creation of output data products produced by the analysis, and provides a modular, flexible software development architecture for implementing algorithms.  These capabilities are supported through the use of a workflow for data integration, a source code generator that produces C, IDL and Python templates, and a graphical interface through which users can efficient define their data input, preprocessing, and output characteristics.  

ADI is used by the 'Atmospheric Radiation Measurement (ARM) Climate Research Facility <http://www.arm.gov>' to process raw data collected from ARM instruments, and to implement scientific algorithms that process one or more of ARM's existing data products to produce new, higher value, data products.

## To Install ADI:

- Install the requisite perl modules. In a terminal, run the commands below. If you have not used cpan before, you will be asked to configure it. Accepting all the defaults will work fine.
  - `sudo cpan JSON::XS`
  - `sudo cpan CGI::Session`
- Install <a href="http://brew.sh/" target="_blank">Homebrew</a> if it's not already installed
- Tap the appropriate Homebrew formulas
  - `brew tap homebrew/science`
  - `brew tap ARM-DOE/adi`
- Install the core ADI libraries:
  - `brew install adi`
- For Python programmers:
  - First, ensure your default version of Python is python 2.7, and has numpy and Cython installed. The easiest way to do this for new users is to install [Anaconda](http://continuum.io/downloads).
  - If you have just installed Anaconda, be sure to restart your bash terminal session so that the new python version has been added to your path. Running `python --version` should show something like 
  
    `Python 2.7.10 :: Anaconda 2.3.0 (x86_64)`.
  - Download the adi python package from [here](https://engineering.arm.gov/~vonderfecht/adi-python-1.0.tar.gz)
  - untar the file, and from inside it, run:
  - `python setup.py install`
  - This should install the ADI libraries for whichever is your default version of python.

## Set Up an Environment for ADI

- Download [adi_home](https://engineering.arm.gov/~vonderfecht/adi_home.tar.gz). This tarfile contains a directory structure to get you started running ADI quickly. Note that you can also configure the directory structure however you like on your own. 
  
  *A bit of explanation here: ADI's VAPs expect certain directories and environment variables to be present, so to run VAPs, you
  must re-create these directories and environment variables. `adi_home` gets this started for you; it contains all the
  directories needed to run vaps, and provides the required environment variables in the file env_vars. You can use the example1
  vap to verify the ADI installation worked properly, and as a place to set up new vaps.*

- untar the file and copy the resulting `adi_home` directory to somewhere you want to work from, say, `~/Documents`
- Enter the untarred `adi_home` directory
- Edit `env_vars` to replace `/path/to/adi_home` with the absolute path to your `adi_home` (whatever `pwd` outputs when you're in `adi_home`)
- Edit `.db_connect` and do the same replacement as in env_vars
- `cat env_vars`, and copy all the `export` commands into your bash terminal, to set up the enviornment. Later you may want to add these environment variables to your `.bash_profile`.
- To test your installation:
  - go into `dev/vap/src/adi_example1`
  - run `make clean; make`. If this fails, something about the installation has gone wrong.
  - run `../../bin/adi_example1_vap -s sbs -f S2 -b 20110401 -e 20110402 -D 2 -R` this should complete successfully.

## To Add Process Definitions to the DSDB:
The Sqlite copy of the database that comes with `adi_home` is a minimal copy containing only a handful of example vap process definitions. To run additional VAPs against your local database, you will need to import their process information.

- Get the process definition from the PCM
  - Go to the <a href="https://engineering.arm.gov/pcm/Main.html" target="_blank">Processing Configuration Manager</a>
    and select the processes tab on the left hand side
  - Type the name of the process you want in the filter at the bottom, or find it by scrolling through the list
  - Double click the name of the process to bring it up on the right hand side
  - Click *Text Export/Import* in the lower right corner, and copy the text that appears to a file on your machine
- Set your enviornment variables as specified in `env_vars` from the last section 
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
