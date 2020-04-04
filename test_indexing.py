import subprocess
import sys
import os


class InvalidConfigError(Exception):
    """
    Exception class made if configuration file is not written correctly
    """
    pass


class DifferentResError(Exception):
    """
    Exception class made if dicts are different
    """
    pass


def extract_output_name(config_file):
    """
    Function that extracts first output filename from configuration
    str -> str, str
    """
    change = "_temp"
    try:
        with open(config_file, "r") as f:
            f.readline()
            name = f.readline()
        name = name[name.index("=") + 2: len(name) - 2]
        temp_name = name[:name.rindex(".")] + change + name[name.rindex("."):]
    except ValueError:
        raise InvalidConfigError
    return name, temp_name


def extract_time(output):
    """
    Function that extracts total time from the output of console
    str -> float
    """
    try:
        temp = "Total: "
        inx = output.index(temp)
        output = float(output[inx + len(temp):])
    except ValueError:
        raise RuntimeError
    return output


def compare_results(paths):
    """
    Function that compares results of two dicts
    list -> None
    list -> Error
    """
    vocabs = [dict(), dict()]
    for file_num in range(len(paths)):
        with open(paths[file_num], "r") as f:
            for line in f:
                # cut words (?)
                sample = line
                sample = sample.split("\t\t:\t")
                sample[1] = sample[1].strip()

                vocabs[file_num][sample[0]] = sample[1]
    # comparing all dicts
    if vocabs.count(vocabs[0]) != len(vocabs):
        raise DifferentResError
    return


def save_new_res(path1, path2):
    """
    Deletes previous temporary result and sets current as temporary
    str, str -> None
    """
    os.remove(path1)
    os.rename(path2, path1)
    return


def main(num_repeat, config_file="config.dat"):
    """
    Function main
    int -> str
    int, str -> str
    int -> Error
    int, str -> Error
    """
    min_time_list = []
    # extracting output name from config
    orig_output, temp_output = extract_output_name(config_file)

    for num in range(num_repeat):
        # executing a file and grabbing output in console
        pipe = subprocess.Popen("cd cmake-build-debug&&./indexing", shell=True, stdout=subprocess.PIPE).stdout
        pipe = pipe.read().decode("utf-8")
        # adding time to list
        min_time_list.append(extract_time(pipe))
        # comparing res if it is not the first one
        if os.path.exists(temp_output):
            compare_results([temp_output, orig_output])
        # interchanging res
        save_new_res(orig_output, temp_output)
    return "Results are equal\n Min time:\t{}".format(min(min_time_list))


if __name__ == "__main__":
    try:
        num_rep = int(sys.argv[1])
        if num_rep < 1:
            raise IndexError

        if len(sys.argv) == 2:
            main(num_rep)
        elif len(sys.argv) == 3:
            name_config = sys.argv[2]
            if not os.path.exists(name_config):
                raise FileNotFoundError
            main(num_rep, name_config)
        else:
            raise TypeError
    except (IndexError, ValueError) as err:
        print("Error: Insert valid number of repetitions")
    except TypeError as err:
        print("Error: Insert valid number of arguments")
    except FileNotFoundError as err:
        print("Error: Needed file is not found")
    except RuntimeError as err:
        print("Error: Output is not correct")
    except InvalidConfigError as err:
        print("Error: Configuration file is invalid")
    except DifferentResError as err:
        print("Results are not equal")
