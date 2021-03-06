#include "log.hpp"

#include <time.h>
#include <fstream>
#include <string>

#include "gtest/gtest.h"

bool base_handle_called = false;
bool full_handle_called = false;

void BaseHandle(std::string entry) { base_handle_called = true; }

void FullHandle(std::pair<unsigned int, std::string> entry) {
  full_handle_called = true;
}

void SetLogs() {
  pessum::ClearLogs();
  pessum::Log(pessum::ERROR, "test 0", "test");
  pessum::Log(pessum::WARNING, "test 1", "test");
  pessum::Log(pessum::TRACE, "test 2", "test");
  pessum::Log(pessum::DEBUG, "test 3", "test");
  pessum::Log(pessum::SUCCESS, "test 4", "test");
  pessum::Log(pessum::INFO, "test 5", "test");
  pessum::Log(pessum::DATA, "test 6", "test");
  pessum::Log(pessum::ERROR, "test 7", "test");
  pessum::Log(pessum::WARNING, "test 8", "test");
  pessum::Log(pessum::TRACE, "test 9", "test");
  pessum::Log(pessum::DEBUG, "test 10", "test");
  pessum::Log(pessum::SUCCESS, "test 11", "test");
  pessum::Log(pessum::INFO, "test 12", "test");
  pessum::Log(pessum::DATA, "test 13", "test");
}

TEST(LogTest, Log) {
  pessum::Log(pessum::DATA, "log entry test", "test");
  std::pair<unsigned int, std::string> log_entry =
      pessum::FGetLog(pessum::NONE);
  EXPECT_EQ(log_entry.first, pessum::DATA);
  EXPECT_EQ(log_entry.second, "[DATA][log entry test][test]");
}

TEST(LogTest, Log_TimeStamp) {
  pessum::SetLogOption(pessum::TIME_STAMP, true);
  pessum::Log(pessum::DATA, "log time stamp test", "test");
  std::pair<unsigned int, std::string> log_entry =
      pessum::FGetLog(pessum::NONE);
  EXPECT_EQ(log_entry.first, pessum::DATA);
  time_t current = time(NULL);
  struct tm* time_info = localtime(&current);
  char buffer[80];
  strftime(buffer, 80, "%T", time_info);
  std::string str(buffer);
  str = "[DATA][" + str + "][log time stamp test][test]";
  EXPECT_EQ(log_entry.second, str);
  pessum::SetLogOption(pessum::TIME_STAMP, false);
}

TEST(LogTest, Log_DateStamp) {
  pessum::SetLogOption(pessum::DATE_STAMP, true);
  pessum::Log(pessum::DATA, "log date stamp test", "test");
  std::pair<int, std::string> log_entry = pessum::FGetLog(pessum::NONE);
  EXPECT_EQ(log_entry.first, pessum::DATA);
  time_t current = time(NULL);
  struct tm* time_info = localtime(&current);
  char buffer[80];
  strftime(buffer, 80, "%D", time_info);
  std::string str(buffer);
  str = "[DATA][" + str + "][log date stamp test][test]";
  EXPECT_EQ(log_entry.second, str);
  pessum::SetLogOption(pessum::DATE_STAMP, false);
}

TEST(LogTest, GetLogSize) {
  SetLogs();
  EXPECT_EQ(pessum::GetLogSize(), 14);
}

TEST(LogTest, ClearLogs) {
  SetLogs();
  EXPECT_NE(pessum::GetLogSize(), 0);
  pessum::ClearLogs();
  EXPECT_EQ(pessum::GetLogSize(), 0);
}

TEST(LogTest, GetLog) {
  SetLogs();
  std::string log_entry = pessum::GetLog(pessum::SUCCESS);
  EXPECT_EQ(log_entry, "[SUCCESS][test 11][test]");
  log_entry = pessum::GetLog(pessum::NONE);
  EXPECT_EQ(log_entry, "[DATA][test 13][test]");
}

TEST(LogTest, FGetLog) {
  SetLogs();
  std::pair<int, std::string> log_entry = pessum::FGetLog(pessum::DEBUG);
  EXPECT_EQ(log_entry.first, pessum::DEBUG);
  EXPECT_EQ(log_entry.second, "[DEBUG][test 10][test]");
}

TEST(LogTest, IGetLog) {
  SetLogs();
  std::string log_entry = pessum::IGetLog(6);
  EXPECT_EQ(log_entry, "[DATA][test 6][test]");
}

TEST(LogTest, IFGetLog) {
  SetLogs();
  std::pair<int, std::string> log_entry = pessum::IFGetLog(7);
  EXPECT_EQ(log_entry.first, pessum::ERROR);
  EXPECT_EQ(log_entry.second, "[ERROR][test 7][test]");
}

TEST(LogTest, VGetLog) {
  SetLogs();
  std::vector<std::string> entries = pessum::VGetLog(1, 4);
  EXPECT_EQ(entries.size(), 4);
  if (entries.size() == 4) {
    EXPECT_EQ(entries[0], "[WARNING][test 1][test]");
    EXPECT_EQ(entries[1], "[TRACE][test 2][test]");
    EXPECT_EQ(entries[2], "[DEBUG][test 3][test]");
    EXPECT_EQ(entries[3], "[SUCCESS][test 4][test]");
  }
}

TEST(LogTest, VFGetLog) {
  SetLogs();
  std::vector<std::pair<unsigned int, std::string>> entries =
      pessum::VFGetLog(1, 4);
  EXPECT_EQ(entries.size(), 4);
  if (entries.size() == 4) {
    EXPECT_EQ(entries[0].first, pessum::WARNING);
    EXPECT_EQ(entries[0].second, "[WARNING][test 1][test]");
    EXPECT_EQ(entries[1].first, pessum::TRACE);
    EXPECT_EQ(entries[1].second, "[TRACE][test 2][test]");
    EXPECT_EQ(entries[2].first, pessum::DEBUG);
    EXPECT_EQ(entries[2].second, "[DEBUG][test 3][test]");
    EXPECT_EQ(entries[3].first, pessum::SUCCESS);
    EXPECT_EQ(entries[3].second, "[SUCCESS][test 4][test]");
  }
}

TEST(LogTest, SetLogHandle) {
  pessum::SetLogHandle(BaseHandle);
  pessum::Log(pessum::INFO, "test", "test");
  EXPECT_EQ(base_handle_called, true);
}

TEST(LogTest, SetFullLogHandle) {
  pessum::SetLogHandle(FullHandle);
  pessum::Log(pessum::INFO, "test", "test");
  EXPECT_EQ(full_handle_called, true);
}

TEST(LogTest, GetTypeStr) {
  std::string str = pessum::GetTypeStr(pessum::DEBUG);
  EXPECT_EQ(str, "DEBUG");
}

TEST(LogTest, SaveLog) {
  SetLogs();
  pessum::SaveLog("tmp.log");
  std::ifstream f("tmp.log");
  EXPECT_TRUE(f.is_open());
  std::vector<std::string> entries = pessum::VGetLog(0, 13);
  if (f.is_open()) {
    std::string line;
    size_t i = 0, j = 0;
    while (getline(f, line)) {
      if (i < entries.size()) {
        EXPECT_EQ(line, entries[i]);
      }
      i++;
      j++;
    }
    EXPECT_EQ(entries.size(), j);
    f.close();
  }
  remove("tmp.log");
}
