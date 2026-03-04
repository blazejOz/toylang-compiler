{
  description = "Development environment for LLVM C++ Compiler";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
  };

  outputs = { self, nixpkgs }:
    let
      system = "x86_64-linux";
      pkgs = import nixpkgs { inherit system; };
    in
    {
      devShells.${system}.default = pkgs.mkShell {
        nativeBuildInputs = with pkgs; [
          cmake
          pkg-config
          llvmPackages.llvm  
          llvmPackages.libllvm
          clang
          clang-tools           
          gdb
          libffi 
          libxml2 
          zlib                 
        ];

        shellHook = ''
          echo "LLVM Compiler Dev Environment Loaded"
          echo "LLVM Version: $(llvm-config --version)"
        '';
      };
    };
}