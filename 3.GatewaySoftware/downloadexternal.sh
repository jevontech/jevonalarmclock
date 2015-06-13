mkdir external/download
cd external/download

if [ ! -f ./gtest-1.7.0.zip  ]; then
    wget https://googletest.googlecode.com/files/gtest-1.7.0.zip 
    unzip  gtest-1.7.0.zip
fi

if [ ! -f ./poco-1.6.0-all.tar.gz ]; then
wget http://pocoproject.org/releases/poco-1.6.0/poco-1.6.0-all.tar.gz
#tar -xzf poco-1.6.0-all.tar.gz
fi

if [ ! -f ./qpid-cpp-0.30.tar.gz ]; then
wget http://apache.mirror1.spango.com/qpid/0.30/qpid-cpp-0.30.tar.gz
tar -xzf qpid-cpp-0.30.tar.gz
fi

#if [ ! -f ./Autowiring-v0.4.3.tar.gz ]; then
#wget -O Autowiring-v0.4.3.tar.gz  https://github.com/leapmotion/autowiring/archive/v0.4.3.tar.gz
#fi

if [ ! -f ./protobuf-cpp.tar.gz ]; then
wget -O protobuf-cpp.tar.gz https://github.com/google/protobuf/releases/download/v3.0.0-alpha-2/protobuf-cpp-3.0.0-alpha-2.tar.gz
fi


