#pragma once

// Simple header-only test framework for Arcane Journey
// No external dependencies — just standard C++17

#include <iostream>
#include <string>
#include <vector>
#include <functional>
#include <cmath>
#include <iomanip>
#include <sstream>

namespace test {

// ---------------------------------------------------------------------------
// Assertions
// ---------------------------------------------------------------------------

inline bool AssertTrue(bool condition, const char* expr, const char* file, int line) {
    if (!condition) {
        std::cerr << "FAIL: " << file << ":" << line << ": Expected true: " << expr << "\n";
    }
    return condition;
}

inline bool AssertFalse(bool condition, const char* expr, const char* file, int line) {
    if (condition) {
        std::cerr << "FAIL: " << file << ":" << line << ": Expected false: " << expr << "\n";
    }
    return !condition;
}

inline bool AssertEqual(double expected, double actual, double epsilon, const char* expStr, const char* actStr, const char* file, int line) {
    if (std::abs(expected - actual) > epsilon) {
        std::cerr << "FAIL: " << file << ":" << line
                  << ": Expected " << expStr << " == " << actStr
                  << " (" << expected << " vs " << actual << ", eps=" << epsilon << ")\n";
        return false;
    }
    return true;
}

inline bool AssertEqual(int expected, int actual, const char* expStr, const char* actStr, const char* file, int line) {
    if (expected != actual) {
        std::cerr << "FAIL: " << file << ":" << line
                  << ": Expected " << expStr << " == " << actStr
                  << " (" << expected << " vs " << actual << ")\n";
        return false;
    }
    return true;
}

inline bool AssertEqual(const std::string& expected, const std::string& actual, const char* expStr, const char* actStr, const char* file, int line) {
    if (expected != actual) {
        std::cerr << "FAIL: " << file << ":" << line
                  << ": Expected " << expStr << " == " << actStr
                  << " (\"" << expected << "\" vs \"" << actual << "\")\n";
        return false;
    }
    return true;
}

inline bool AssertNear(float expected, float actual, float epsilon, const char* expStr, const char* actStr, const char* file, int line) {
    return AssertEqual(expected, actual, epsilon, expStr, actStr, file, line);
}

#define ASSERT_TRUE(expr)       test::AssertTrue((expr), #expr, __FILE__, __LINE__)
#define ASSERT_FALSE(expr)      test::AssertFalse((expr), #expr, __FILE__, __LINE__)
#define ASSERT_EQ(a, b)         test::AssertEqual((a), (b), #a, #b, __FILE__, __LINE__)
#define ASSERT_EQ_F(a, b, eps)  test::AssertNear((a), (b), (eps), #a, #b, __FILE__, __LINE__)
#define ASSERT_STR_EQ(a, b)     test::AssertEqual((a), (b), #a, #b, __FILE__, __LINE__)

// ---------------------------------------------------------------------------
// Test Case & Suite
// ---------------------------------------------------------------------------

struct TestResult {
    std::string name;
    bool passed;
    std::string message;
};

class TestSuite {
public:
    using TestFn = std::function<void()>;

    static TestSuite& instance() {
        static TestSuite s;
        return s;
    }

    void addTest(const std::string& suiteName, const std::string& testName, TestFn fn) {
        tests_.push_back({suiteName + "::" + testName, std::move(fn)});
    }

    int runAll() {
        int passed = 0;
        int failed = 0;

        std::cout << "\n========================================\n";
        std::cout << "Running " << tests_.size() << " tests...\n";
        std::cout << "========================================\n\n";

        for (auto& test : tests_) {
            bool ok = true;
            try {
                test.fn();
            } catch (const std::exception& e) {
                ok = false;
                std::cerr << "EXCEPTION in " << test.name << ": " << e.what() << "\n";
            } catch (...) {
                ok = false;
                std::cerr << "UNKNOWN EXCEPTION in " << test.name << "\n";
            }

            if (ok) {
                std::cout << "  [PASS] " << test.name << "\n";
                passed++;
            } else {
                std::cout << "  [FAIL] " << test.name << "\n";
                failed++;
            }
        }

        std::cout << "\n========================================\n";
        std::cout << "Results: " << passed << " passed, " << failed << " failed\n";
        std::cout << "========================================\n";

        return failed == 0 ? 0 : 1;
    }

private:
    struct TestEntry {
        std::string name;
        TestFn fn;
    };
    std::vector<TestEntry> tests_;
};

// Macro to register a test
#define TEST(suite, name) \
    static void Test_##suite##_##name(); \
    struct TestRegistrar_##suite##_##name { \
        TestRegistrar_##suite##_##name() { \
            test::TestSuite::instance().addTest(#suite, #name, Test_##suite##_##name); \
        } \
    }; \
    static TestRegistrar_##suite##_##name registrar_##suite##_##name; \
    static void Test_##suite##_##name()

} // namespace test