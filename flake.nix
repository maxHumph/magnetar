{
  description = "Magnetar development environment";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
  };

  outputs = { self, nixpkgs }:
    let
      systems = [
        "x86_64-linux"
        "aarch64-linux"
      ];
      forAllSystems = f:
        nixpkgs.lib.genAttrs systems (system:
          f (import nixpkgs { inherit system; }));
    in {
      devShells = forAllSystems (pkgs: {
        default = pkgs.mkShell {
          packages = with pkgs; [
            bash
            clang
            gnumake
            pkg-config

            vulkan-loader
            vulkan-headers
            vulkan-validation-layers
            vulkan-tools
            vulkan-utility-libraries

            xorg.libX11
            xorg.libXrandr
            xorg.libXinerama
            xorg.libXcursor
            xorg.libXi
            xorg.libXxf86vm
            libxkbcommon
            xorg.libxcb
          ];

          shellHook = ''
            echo "Environment loaded..."
            export VK_LAYER_PATH=${pkgs.vulkan-validation-layers}/share/vulkan/explicit_layer.d
          '';
        };
      });
    };
}
