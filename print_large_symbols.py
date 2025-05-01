# extra_script.py
import os
from SCons.Script import Import

# To see saved sizes:
# cat .pio/build/nanoatmega328new/symbol_sizes.txt

previewSymbolCount = 20


Import("env")

# Declare the ELF and output map as SCons File nodes
elf_node = env.File("$BUILD_DIR/${PROGNAME}.elf")
map_node = env.File("$BUILD_DIR/symbol_sizes.txt")

def generate_size_report(source, target, env):
    # Expand to full paths
    elf_path = env.subst(str(elf_node))
    map_path = env.subst(str(map_node))
    print(f"Generating symbol size map: {map_path}")
    # Run avr-nm (bundled with PlatformIO) and dump sorted sizes
    cmd = f"avr-nm -C -l -S --size-sort \"{elf_path}\" > \"{map_path}\""
    result = os.system(cmd)
    
    # If it succeeded, print the top 10 symbols:
    if result == 0:
        with open(map_path, 'r') as f:
            lines = f.readlines()
            # lines.reverse() # Go to bottom of file where largest are
            # topLines = lines[:previewSymbolCount]
            topLines = lines[len(lines) - previewSymbolCount:]
            print("\n")
            print("Top", previewSymbolCount, "symbols:")
            for line in topLines:
                print(line.strip())
            print("\n")
    
    return result

# Attach post‐build action to the ELF node
env.AddPostAction(elf_node, generate_size_report)
