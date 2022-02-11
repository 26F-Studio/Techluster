# Techluster

The new distributed backends for [Techmino](https://github.com/26F-Studio/Techmino)

<p style="text-align:center">
    <img src="https://img.shields.io/github/languages/code-size/26F-Studio/Techluster.svg?style=flat-square" alt="Code size"/>
    <img src="https://img.shields.io/github/repo-size/26F-Studio/Techluster.svg?style=flat-square" alt="GitHub repo size"/>
</p>

## Design Graph

[![Design Graph](http://assets.processon.com/chart_image/615fd6410e3e747620f50b70.png)](https://www.processon.com/view/link/611977a06376893a9f00fb77)

## Work Progress

### Bot Gateway

#### Todo

- Manage requests from gaming nodes and allocate bot node to transfer Nodes
- Implement multi-thread bot service

#### In progress

#### Finished

---

### Connect Gateway

#### Todo

- Receive user location info and allocate proper transfer node.

#### In progress

-[X] Allocate various server nodes
-[X] Monitor server nodes and decide which one to be allocated
-[X] Authenticate new server nodes

#### Finished

---

### Gaming Gateway

#### Todo

#### In progress

- Connect to clients through websocket
- Resolve clients' message by action number
- Create/Remove gaming room
- Change room data/info
- Join/Leave gaming room
- Kick players
- Get player's network conditions
- Get player's game data

#### Finished

---

### Message Gateway

#### Todo

- Broadcast global message to clients
- Allow clients to pull notices from the server
- Provide global chatting service and private message
- Provide in room chatting service

#### In progress

#### Finished

---

### Transfer Gateway

#### Todo

- Get player map from Gaming node and connect to clients
- Receive and broadcast gaming data
- Calculate versus statistics

#### In progress

#### Finished

---

### User Gateway

#### Todo

- Use reCaptcha for web requests

#### In progress

-[ ] Check if the access token is valid
-[ ] Refresh access token and refresh token (if necessary)
-[ ] Send verification code through email to newly registered user
-[ ] Login with verification code or password
-[ ] Reset password with verification code
-[ ] Change email with access token and verification code
-[ ] Get/Update user info and avatar
-[ ] Get/Update user data

#### Finished

---

## Develop

### Requirements

#### Windows

- CMake 3.20.x and above
- Git
- Visual Studio 2019 and above (With English language pack)
- PostgresSQL server 14 and above
- Redis server 3.2.x and above (Use WSL or remote linux server)

#### linux

- CMake 3.20.x and above
- Git
- GCC 9 or Clang 9 and above (10 if you want to use coroutine)
- PostgresSQL server 14 and above
- Redis server 3.2.x and above

### Steps

1. Clone the repository

```bash
git clone --recursive https://github.com/26F-Studio/Techluster
```

2. Bootstrap `Vcpkg`

```bash
# Linux
cd ./Techluster/vcpkg
chmod +x ./bootstrap-vcpkg.sh
./bootstrap-vcpkg.sh

# Windows
cd ./Techluster/vcpkg
./bootstrap-vcpkg.bat
```

3. Create config.json from the config.json.example in each subdirectory

4. Create build directory and run CMake

```bash
# Linux
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Debug -G "CodeBlocks - Unix Makefiles" .. 
cmake --build .. --target all

# Windows
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Debug -G "CodeBlocks - NMake Makefiles" .. 
cmake --build .. --target all
```

5. Run each server nodes
