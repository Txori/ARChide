import os
import csv

def move_hidden_games(input_file, config_file, delete_folder):
    # Check if necessary files exist
    if not os.path.exists(input_file):
        print(f"Error: {input_file} not found.")
        input("Press Enter to exit")
        exit()

    if not os.path.exists(config_file):
        print(f"Error: {config_file} not found.")
        input("Press Enter to exit")
        exit()

    # Read the configuration file into a dictionary
    system = {}
    with open(config_file, 'r') as config:
        for line in config:
            key, value = line.strip().split('=')
            system[key.strip()] = value.strip()

    # Create the delete folder if it doesn't exist
    if not os.path.exists(delete_folder):
        os.makedirs(delete_folder)

    # Process the input file - First pass to check missing systems
    missing_systems = set()
    with open(input_file, 'r', encoding='utf-8', newline='') as csvfile:
        reader = csv.DictReader(csvfile)

        for row in reader:
            system_name = row['System']
            hidden = row['Hidden']

            if hidden.upper() == 'HIDDEN':
                # Check if the system is configured
                if system_name not in system:
                    missing_systems.add(system_name)

    # If missing systems are found, print and stop the script
    if missing_systems:
        print("Error: Missing systems in config file:")
        for missing_system in missing_systems:
            print(f" - {missing_system}")
        input("Press Enter to exit")
        exit()

    # Second pass to actually move hidden games
    with open(input_file, 'r', encoding='utf-8', newline='') as csvfile:
        reader = csv.DictReader(csvfile)
        
        for row in reader:
            try:
                system_name = row['System']
                filename = row['Filename']
                hidden = row['Hidden']

                if hidden.upper() == 'HIDDEN':
                    # Get the folder name for the system
                    system_folder = system.get(system_name)
                    if system_folder:
                        # Build the full path to the file
                        file_path = os.path.join(system_folder, filename)

                        # Build the destination path in the _hidden directory
                        destination_path = os.path.join(delete_folder, system_folder, filename)

                        try:
                            # Create the system-specific folder in the delete_folder if it doesn't exist
                            system_folder_path = os.path.join(delete_folder, system_folder)
                            if not os.path.exists(system_folder_path):
                                os.makedirs(system_folder_path)

                            # Move the file to the destination path
                            os.rename(file_path, destination_path)
                            print(f"Moved: {file_path}")

                        except FileNotFoundError:
                            print(f"File not found: {file_path}")

                        except Exception as e:
                            print(f"Error moving file: {e}")

                    else:
                        print(f"System not found in config file: {system_name}")

            except Exception as e:
                print(f"Error processing row: {e}")

if __name__ == "__main__":
    
    input_file = "games.csv"
    config_file = "config.txt"
    delete_folder = "_hidden"

    move_hidden_games(input_file, config_file, delete_folder)
    print("\nHidden games moved to _hidden folder. Check the results above.")
    input("Press Enter to exit")
