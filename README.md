# CSE305 Project: Parallel Crawler

## Team Members
Junyuan Wang, Yubo Cai.

## Potential Problem
If you find CA problem for libcurl, please pass the parameter "--lab-machine=1" and put the [ca-bundle.crt](./ca-bundle.crt) to the **build** folder. This is required because of the permission problem for the libcurl to get the right certificate. Therefore, we need to manually put it.



## Depolyment

#### 1. Environment Requirements:
his project is only tested on the Ubuntu X86 system. However, we included all the source code of external libraries, `Libcurl` and `Cpplog`, allowing for compilation on other platforms. **CMake**, **g++**, and **OpenSSL** are **required** to be installed on the machine.

#### 2. Compilation:
  - Creating a build folder under the root directory of the project and entering this folder.
```bash
mkdir build && cd build
```
- Running the following command to compile the project.
  
```bash
cmake .. && make
```
- If there is no compilation error, the executable files should be located in the `build` folder named `ParallelCrawler`. Start this program by simply running the following command and passing the parameters. (Be sure to have the right permission)

```bash
./ParallelCrawler --url=https://example.com
```

For parameters information, please check our [[report]](CSE305_Project.pdf) to see more details. We here provide an example of the command to use the package, crawling the Wikipedia English Pages and set the time limit to 60 seconds.
```bash
./ParallelCrawler --url=https://en.wikipedia.org/wiki/Main_Page --threads=32 --output=found_urls.txt --max-urls=0 --max-time=60 --debug=1 cache-type=1
```
