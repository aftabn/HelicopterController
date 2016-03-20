using System;
using System.Collections.Generic;

namespace Libs.Utilities
{
    public static class DateTimeExtensions
    {
        public static List<DateTime> DaysOfTheMonth(this DateTime date)
        {
            var currentDay = date.StartOfMonth();
            var startOfNextMonth = date.StartOfNextMonth();

            var list = new List<DateTime>();

            while (currentDay < startOfNextMonth)
            {
                list.Add(currentDay);
                currentDay = currentDay.AddDays(1);
            }

            return list;
        }

        public static DateTime EndOfDay(this DateTime date)
        {
            return date.Date.AddDays(1).AddTicks(-1);
        }

        // http://stackoverflow.com/questions/11/how-do-i-calculate-relative-time
        public static string GetTimeAgo(this TimeSpan timeSpan)
        {
            var totalSeconds = timeSpan.TotalSeconds;

            const int second = 1;
            const int minute = 60 * second;
            const int hour = 60 * minute;
            const int day = 24 * hour;
            const int month = 30 * day;

            if (totalSeconds < 0)
            {
                return "not yet";
            }
            if (totalSeconds < 1 * minute)
            {
                return timeSpan.Seconds == 1 ? "one second ago" : timeSpan.Seconds + " seconds ago";
            }
            if (totalSeconds < 2 * minute)
            {
                return "a minute ago";
            }
            if (totalSeconds < 45 * minute)
            {
                return timeSpan.Minutes + " minutes ago";
            }
            if (totalSeconds < 90 * minute)
            {
                return "an hour ago";
            }
            if (totalSeconds < 24 * hour)
            {
                return timeSpan.Hours + " hours ago";
            }
            if (totalSeconds < 30 * day)
            {
                return (timeSpan.Days + 1) + " days ago";
            }

            if (totalSeconds < 12 * month)
            {
                var months = Convert.ToInt32(Math.Floor((double)timeSpan.Days / 30));
                return months <= 1 ? "one month ago" : months + " months ago";
            }
            else
            {
                var years = Convert.ToInt32(Math.Floor((double)timeSpan.Days / 365));
                return years <= 1 ? "one year ago" : years + " years ago";
            }
        }

        public static DateTime StartOfDay(this DateTime date)
        {
            return date.Date;
        }

        // http://stackoverflow.com/questions/38039/how-can-i-get-the-datetime-for-the-start-of-the-week
        public static DateTime StartOfWeek(this DateTime dateTime, DayOfWeek startOfWeek = DayOfWeek.Sunday)
        {
            var daysDifference = dateTime.DayOfWeek - startOfWeek;

            if (daysDifference < 0)
            {
                daysDifference += 7;
            }

            return dateTime.AddDays(-1 * daysDifference).Date.StartOfDay();
        }

        public static DateTime StartOfMonth(this DateTime date)
        {
            return new DateTime(date.Year, date.Month, 1);
        }

        public static DateTime StartOfNextDay(this DateTime date)
        {
            return date.StartOfDay().AddDays(1);
        }

        public static DateTime StartOfNextWeek(this DateTime date)
        {
            return date.StartOfWeek().AddDays(7);
        }

        public static DateTime StartOfHour(this DateTime date)
        {
            return new DateTime(date.Year, date.Month, date.Day, date.Hour, 0, 0);
        }

        public static DateTime StartOfNextHour(this DateTime date)
        {
            return date.AddHours(1.0).StartOfHour();
        }

        public static DateTime StartOfPreviousMonth(this DateTime date)
        {
            return date.StartOfMonth().AddMonths(-1);
        }

        public static DateTime StartOfNextMonth(this DateTime date)
        {
            return date.StartOfMonth().AddMonths(1);
        }

        public static bool IsSameDate(this DateTime date, DateTime dateToCompare)
        {
            return date.Year == dateToCompare.Year && date.Month == dateToCompare.Month && date.Day == dateToCompare.Day;
        }

        public static bool IsToday(this DateTime date)
        {
            return date.IsSameDate(DateTime.Now);
        }
    }
}