#include <Arduino.h>
#include <NTPClient.h>
#include <RTClib.h>

class ModifiedDateTime
{
public:
    int second;
    int minute;
    int hour;
    int day;
    int month;
    int year;
};

class ModifiedNTPClient : public NTPClient
{
public:
    /**
     * Constructor for ModifiedNTPClient
     */
    ModifiedNTPClient(UDP& udp, const char* poolServerName = "pool.ntp.org", int timeOffset = 0, int updateInterval = 60000)
        : NTPClient(udp, poolServerName, timeOffset, updateInterval) {}
    /**
     * Get date time as a struct which contains
     * Year, Month, Date, Hours, Minutes, Seconds
     */
    ModifiedDateTime getDateTime() const;

    /**
     * Return the date time as a String with the given format (Ex: %Y/%m/%d %H:%M:%S)
     */
    String getFormattedDateTime(const char *dateTimeFormat) const;
};

ModifiedDateTime ModifiedNTPClient::getDateTime() const
{
    struct tm *ts;
    time_t rawTime = this->getEpochTime();
    ts = localtime(&rawTime);
    ModifiedDateTime dt = {ts->tm_sec, ts->tm_min, ts->tm_hour, ts->tm_mday, (ts->tm_mon + 1), (ts->tm_year + 1900)};
    return dt;
}

String ModifiedNTPClient::getFormattedDateTime(const char *dateTimeFormat) const
{
    struct tm *ts;
    time_t rawTime = this->getEpochTime();
    ts = localtime(&rawTime);
    char buf[64];
    strftime(buf, sizeof(buf), dateTimeFormat, ts);
    return String(buf);
}