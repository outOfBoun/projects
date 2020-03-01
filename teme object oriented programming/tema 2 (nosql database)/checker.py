import os
import filecmp
from subprocess import Popen,PIPE,STDOUT,call


TEST_NO = 10
TEST_NO_BONUS = 2

def clean_solution():
    os.system("rm -rf output")
    os.system("make clean")


def build_solution():
    print "Building Tema2..."
    os.system("make build")


def run_all_tests():
    for i in range(0, TEST_NO):
        test_file = "test" + str(i + 1)
        os.system("make INPUT_FILE=" + test_file + " run")

    write_cleanup_command()
    for i in range(0, TEST_NO_BONUS):
        test_file = "testb" + str(i + 1)
        os.system("make INPUT_FILE=" + test_file + " run")

def copy_output_file():
    os.system("mkdir -p output")
    for i in range(0, TEST_NO):
        tests_output_file = "test" + str(i + 1) + "_out"
        os.system("mv " + tests_output_file + " output")

    for i in range(0, TEST_NO_BONUS):
        test_file_output_b = "testb" + str(i + 1) + "_out"
        os.system("mv " + test_file_output_b + " output")


def compare():
    total_score = 0
    for i in range(0, TEST_NO):
        local_score = 0
        test_ref_file = "ref/test" + str(i + 1) + "_out"
        test_output_file = "output/test" + str(i + 1) + "_out"

        if filecmp.cmp(test_output_file, test_ref_file):
            local_score = 10

        print(
            "Test " + str(i + 1) + "......................................" + str(local_score) + "/10")
        total_score += local_score

    print("Total Score:......................................" + str(total_score) + "/100")

def compare_bonus():
    total_bonus = 0
    for i in range(0, TEST_NO_BONUS):
        local_score = 0
        test_file_output_b = "output/testb" + str(i + 1) + "_out"
        test_ref_file_b = "ref/testb" + str(i + 1) + "_out"

        if filecmp.cmp(test_file_output_b, test_ref_file_b):
            local_score = 10

            print(
                "Test Bonus" + str(i + 1) + "......................................" + str(local_score) + "/10")
        total_bonus += local_score

    print("Total Bonus Score:......................................" + str(total_bonus) + "/" + str(10 * TEST_NO_BONUS))

def write_cleanup_command():
    file_bonus = open("testb1", "a")
    file_bonus.write("CLEANUP Tema2 " + str(get_nanos()))
    file_bonus.write("\nSNAPSHOTDB")
    file_bonus.close()

def cleanup_bonus():
    os.system('head -n -2  testb1 > testb1_temp')
    os.system('mv testb1_temp testb1')

def get_nanos():
    proc=Popen('date +%s%N', shell=True, stdout=PIPE,)
    nanos = long(proc.communicate()[0])
    return nanos

def main():
    print("Building Tema2...")

    clean_solution()
    build_solution()

    run_all_tests()

    copy_output_file()

    compare()
    compare_bonus()

    cleanup_bonus()


if __name__ == '__main__':
    main()
