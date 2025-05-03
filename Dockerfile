# Use the official Ubuntu image as the base
FROM ubuntu:latest

# Set the working directory inside the container
WORKDIR /app

# Install necessary dependencies
RUN apt-get update && apt-get install -y \
    g++ \
    cmake \
    make \
    git \
    libsfml-dev \
    libx11-dev \
    libxcursor-dev \
    libxrandr-dev \
    libxi-dev \
    libgl1-mesa-dev \
    libudev-dev \
    libopenal-dev \
    libflac-dev \
    libvorbis-dev \
    && apt-get clean

# Copy project files into the container
COPY . .

# Build the project using CMake
RUN mkdir build && cd build && cmake .. && make

# Set the default command to run the executable
CMD ["./build/sfml-demo"]
