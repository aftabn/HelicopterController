using System;

namespace Libs.Utilities
{
    public class RelativeTime
    {
        public static string GetTimeAgoString(DateTimeOffset startTime, DateTimeOffset endTime)
        {
            return (endTime - startTime).GetTimeAgo();
        }

        public static string GetTimeAgoString(DateTime startTime, DateTime endTime)
        {
            return (endTime - startTime).GetTimeAgo();
        }
    }
}