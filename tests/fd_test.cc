#include <cstddef>
#include <fcntl.h>
#include <gtest/gtest.h>
#include <limits>
#include <string>
#include <sys/socket.h>

#include "../src/include/fd.h"

TEST(TEST_CREATE, create_test) {
    auto f = socket(AF_INET, SOCK_STREAM, 0);
    nt::file_discriptor fd(f);
    ASSERT_EQ(true, !fd.is_closed());

    fd.duplicate();
    ASSERT_EQ(true, !fd.is_closed());
}

TEST(TEST_READ, read_test) {
    auto zero_f = open("/dev/zero", O_RDONLY);
    nt::file_discriptor fd(zero_f);
    ASSERT_EQ(true, !fd.is_closed());

    std::string buf;
    auto read_len = fd.read(buf, 128);
    ASSERT_EQ(128, read_len);
    ASSERT_EQ("", buf);

    read_len = fd.read(buf, 512);
    ASSERT_EQ(512, read_len);
    ASSERT_EQ("", buf);

    read_len = fd.read(buf, 1024);
    ASSERT_EQ(1024, read_len);
    ASSERT_EQ("", buf);

    read_len = fd.read(buf, std::numeric_limits<size_t>::max());
    ASSERT_EQ(1024 * 1024, read_len);
    ASSERT_EQ("", buf);

    for (int i = 0; i < 10e5; i += 100) {
        read_len = fd.read(buf, 1);
        ASSERT_EQ(1, read_len);
        ASSERT_EQ("", buf);
    }

    for (int i = 0; i < 10e5; i += 100) {
        if (i == 1024 * 1024) {
            i %= 1024 * 1024;
        }
        read_len = fd.read(buf, i);
        ASSERT_EQ(i, read_len);
        ASSERT_EQ("", buf);
    }
}

TEST(TEST_WRITE, write_test) {
    auto null_f = open("/dev/null", O_WRONLY);
    nt::file_discriptor fd(null_f);
    ASSERT_EQ(true, !fd.is_closed());

    auto write_len = fd.write("hello world", 12);
    ASSERT_EQ(12, write_len);
    
    std::string buf(128, 'a');
    write_len = fd.write(buf, 64);
    ASSERT_EQ(64, write_len);

    write_len = fd.write(buf, buf.size());
    ASSERT_EQ(buf.size(), write_len);

    for (int i = 0; i < 10e5; i++) {
        write_len = fd.write("a", 1);
        ASSERT_EQ(1, write_len);
    }
    for (int i = 0; i < 10e5; i++) {
        if (i == 1024 * 1024) {
            i %= 1024 * 1024;
        }
        write_len = fd.write("a", i);
        ASSERT_EQ(i, write_len);
    }
}

TEST(TEST_DUP, duplicate_test) {
    auto null_f = open("/dev/null", O_WRONLY);
    nt::file_discriptor fd(null_f);
    ASSERT_EQ(true, !fd.is_closed());

    {
        auto fd1 = fd.duplicate();
        ASSERT_EQ(true, !fd.is_closed());
        ASSERT_EQ(fd1.get_fd(), fd.get_fd());
    }
    ASSERT_EQ(true, !fd.is_closed());
}

TEST(TEST_BLK, blocking_test) {
    auto null_f = open("/dev/null", O_WRONLY);
    nt::file_discriptor fd(null_f);
    ASSERT_EQ(true, !fd.is_closed());

    ASSERT_TRUE(fd.is_blocking());
    fd.set_blocking();
    ASSERT_EQ(false, fd.is_blocking());

    ssize_t write_len;
    for (int i = 0; i < 10e5; i++) {
        write_len = fd.write("a", 1);
        ASSERT_EQ(1, write_len);
    }

    fd.set_blocking();
    ASSERT_EQ(true, fd.is_blocking());

    for (int i = 0; i < 10e5; i++) {
        write_len = fd.write("a", 1);
        ASSERT_EQ(1, write_len);
    }
}

GTEST_API_ int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
