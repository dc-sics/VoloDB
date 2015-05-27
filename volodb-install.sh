if [ "$EUID" -ne 0 ]
  then echo "Please run as root"
  exit
fi

#Installing compiling projects
echo "Compiling volodb and depdendent projects"
if ! make --silent -C volodbserver; then
    echo "VoloDB server compilation failed"
        exit 1
fi

if ! make --silent -C volodbclientapp; then
    echo "VoloDB client application compilation failed"
    exit 1
fi

if ! make --silent -C volodb; then
    echo "VoloDB server library compilation failed"
    exit 1
fi


#Installing header files
echo "Installing header files"
mkdir -p /usr/local/include/volodb
if [ $? -ne 0 ]; then
    echo "Failed to create folder in /usr/local/include/"
    exit 1
fi  

rsync -az --include '*/' --include '*.h' --exclude "*"  --prune-empty-dirs volodb-common/ /usr/local/include/volodb/common
if [ $? -ne 0 ]; then
    echo "Failed to install common header"
    exit 1
fi  

rsync -az --include '*/' --include '*.h' --exclude "*"  --prune-empty-dirs volodb-client/ /usr/local/include/volodb/client
if [ $? -ne 0 ]; then
    echo "Failed to install volodb client headers"
    exit 1
fi

rsync -az --include '*/' --include '*.h' --exclude "*"  --prune-empty-dirs volodbserver/ /usr/local/include/volodb/volodbserver
if [ $? -ne 0 ]; then
    echo "Failed to install volodb server headers"
    exit 1
fi

#Installing lib files
echo "Installing lib files"
mkdir -p /usr/local/lib/volodb
if [ $? -ne 0 ]; then
    echo "Failed to create folder in /usr/local/lib/"
    exit 1
fi

cp volodb-common/dist/Release/*/libvolodb-common.a /usr/local/lib/volodb/
if [ $? -ne 0 ]; then
    echo "Failed to install common lib"
    exit 1
fi

cp volodb-client/dist/Release/*/libvolodb-client.a /usr/local/lib/volodb/
if [ $? -ne 0 ]; then
    echo "Failed to install client lib"
    exit 1
fi  

cp volodb/dist/Release/*/libvolodb.a /usr/local/lib/volodb/
if [ $? -ne 0 ]; then
    echo "Failed to install volodb server lib"
    exit 1
fi


#Installing Binaries
echo "Installing binaries"
cp volodbserver/dist/Release/*/volodbserver /usr/local/bin
if [ $? -ne 0 ]; then
    echo "Failed to install volodb server binary"
    exit 1
fi

cp volodbclientapp/dist/Release/*/volodbclientapp /usr/local/bin
if [ $? -ne 0 ]; then
    echo "Failed to install volodb client application"
    exit 1
fi

#Installing configuration files
echo "Installing configuration files"
cp volodbserver/config.props /etc/volodb.conf
if [ $? -ne 0 ]; then
    echo "Failed to install volodb server configuration file"
    exit 1
fi

cp volodbclientapp/test.cfg /etc/volodb-client.conf
if [ $? -ne 0 ]; then
    echo "Failed to install volodb client configuration file"
    exit 1
fi

echo ""
echo "Installed successfully!!!"
echo ""
echo "Headers installed in: /usr/local/include/volodb"
echo "Libraries installed in: /usr/local/lib/volodb"
echo "Binaries installed in: /usr/local/bin"
echo "Config files installed in: /etc/"
echo ""
echo ""