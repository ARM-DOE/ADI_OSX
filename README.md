# ADI
A cross-platform version of ADI that can compile on linux and OSX

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
