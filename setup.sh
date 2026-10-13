#!/usr/bin/env bash
# One-command setup for Ubuntu / WSL:
# installs dependencies, builds the libxenon toolchain, builds hello360.
# Safe to re-run; it skips the toolchain if it's already built.
set -euo pipefail

export DEVKITXENON=/usr/local/xenon

echo "==> Installing dependencies (needs sudo)"
sudo apt-get update
sudo apt-get install -y build-essential git wget texinfo flex bison \
  libgmp-dev libmpfr-dev libmpc-dev libssl-dev zlib1g-dev libncurses-dev

if [ ! -x "$DEVKITXENON/bin/xenon-g++" ]; then
  echo "==> Building libxenon toolchain (this can take an hour or more)"
  sudo mkdir -p "$DEVKITXENON"
  sudo chown "$USER" "$DEVKITXENON"
  rm -rf "$HOME/libxenon"
  git clone https://github.com/Free60Project/libxenon.git "$HOME/libxenon"
  ( cd "$HOME/libxenon/toolchain" &&
    ./build-xenon-toolchain toolchain &&
    ./build-xenon-toolchain libxenon )
else
  echo "==> Toolchain already built, skipping"
fi

if ! grep -q "DEVKITXENON" "$HOME/.bashrc"; then
  echo "==> Saving environment to ~/.bashrc"
  {
    echo 'export DEVKITXENON=/usr/local/xenon'
    echo 'export PATH=$PATH:$DEVKITXENON/bin'
  } >> "$HOME/.bashrc"
fi

export PATH="$PATH:$DEVKITXENON/bin"
echo "==> Building hello360"
make

echo
echo "Done: hello360.elf32 is in $(pwd)"
