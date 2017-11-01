apt install doxygen
wget https://github.com/assimp/assimp/archive/v4.0.1.tar.gz -P ./vendor

# boost
bjam toolset=gcc --with-thread --with-signals --with-chrono --with-system --with-timer --with-date_time --with-filesystem --link=static --ignore-site-config