using Helicopter.Core.Controller;
using Helicopter.Core.Sessions;
using Helicopter.Core.Settings;
using Helicopter.Model;
using Libs.Utilities;
using log4net;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Data.SqlClient;
using System.Linq;
using System.Security.Cryptography;
using System.Text;
using System.Threading.Tasks;

namespace Helicopter.Core
{
    public static class DatabaseManager
    {
        public static readonly SHA1 HashFunction = SHA1.Create();
        private const int INT_MaxRowsReturned = 200;
        private static readonly ILog Log = LogManager.GetLogger(System.Reflection.MethodBase.GetCurrentMethod().DeclaringType);

        public static string GetSHA1Hash(string str)
        {
            var buffer = Encoding.ASCII.GetBytes(str);
            var hash = HashFunction.ComputeHash(buffer);

            return String.Concat(hash.Select(x => x.ToString("X2")).ToArray()).ToUpper();
        }

        // Gets a collection of DatabaseRow objects by searching through the database using the specified search paramaters
        // These DatabaseRow objects can be easily bound to and displayed from a datagrid
        public static ObservableCollection<DatabaseRow> GetQueriedRecords(bool isSearchingById, int recordId, DateTime date)
        {
            using (var context = new HelicopterModelEntities())
            {
                IQueryable<SessionRecord> records;
                if (isSearchingById)
                {
                    records = context.SessionRecords.Where(x => x.Id <= recordId).OrderByDescending(x => x.Id).Take(INT_MaxRowsReturned);
                }
                else
                {
                    date = date.AddDays(1);
                    records = context.SessionRecords.Where(x => x.StartTime <= date).OrderByDescending(x => x.Id).Take(INT_MaxRowsReturned);
                }

                var queriedRecords = LoadRecords(records);

                return queriedRecords;
            }
        }

        public static bool IsDatabaseAlive()
        {
            try
            {
                const string connectionString = "Data Source=AFTABS-DELL; initial catalog=HelicopterModel; persist security info=True; user id=HelicopterControllerSoftware; password=tooEZ2tune; MultipleActiveResultSets=True; App=EntityFramework";

                Log.DebugFormat("Connecting to: {0}", connectionString);

                using (var connection = new SqlConnection(connectionString))
                {
                    var query = "select 1";
                    Log.DebugFormat("Executing: {0}", query);

                    var command = new SqlCommand(query, connection);

                    connection.Open();
                    Log.DebugFormat("SQL Connection successful.");

                    command.ExecuteScalar();
                    Log.DebugFormat("SQL Query execution successful.");

                    return true;
                }
            }
            catch (Exception ex)
            {
                Log.DebugFormat("Failure: {0}", ex.Message);
                return false;
            }
        }

        public static void UpdateSessionComment(int recordId, string comment)
        {
            using (var context = new HelicopterModelEntities())
            {
                var sessionRecord = context.SessionRecords.Single(x => x.Id == recordId);
                sessionRecord.Comment = comment;
                context.SaveChanges();
            }
        }

        public static SessionRecord GetSessionRecord(int recordId, HelicopterModelEntities context)
        {
            return context.SessionRecords.SingleOrDefault(x => x.Id == recordId);
        }

        public static SessionRecord CreateNewSessionRecord(Session session, HelicopterSettings settings)
        {
            SessionRecord sessionRecord;
            var yaw = session.YawDataSeries;
            var tilt = session.TiltDataSeries;

            using (var context = new HelicopterModelEntities())
            {
                var settingsRecordId = UpdateSettingsRecord(settings, context);

                sessionRecord = new SessionRecord
                {
                    SettingsId = settingsRecordId,
                    StartTime = session.StartTime,
                    EndTime = session.EndTime,
                    Comment = String.Empty
                };

                var yawRecord = new ControllerRecord
                {
                    MotorType = yaw.MotorType.ToString(),
                    DriverType = yaw.MotorDriver.ToString(),
                    CWProportionalGain = yaw.CWProportionalGain,
                    CWIntegralGain = yaw.CWIntegralGain,
                    CWDerivativeGain = yaw.CWDerivativeGain,
                    CCWProportionalGain = yaw.CCWProportionalGain,
                    CCWIntegralGain = yaw.CCWIntegralGain,
                    CCWDerivativeGain = yaw.CCWDerivativeGain,
                    IntegralWindupThreshold = yaw.IWindupThreshold,
                    OutputRateLimit = yaw.OutputRateLimit,
                    MeasurementRecords = yaw.ControllerData.Select(x => new MeasurementRecord
                    {
                        TimeStamp = x.TimeStamp,
                        SetPoint = x.SetPoint,
                        CurrentAngle = x.CurrentAngle
                    }).ToList()
                };

                var tiltRecord = new ControllerRecord
                {
                    MotorType = tilt.MotorType.ToString(),
                    DriverType = tilt.MotorDriver.ToString(),
                    CWProportionalGain = tilt.CWProportionalGain,
                    CWIntegralGain = tilt.CWIntegralGain,
                    CWDerivativeGain = tilt.CWDerivativeGain,
                    IntegralWindupThreshold = tilt.IWindupThreshold,
                    OutputRateLimit = tilt.OutputRateLimit,
                    MeasurementRecords = tilt.ControllerData.Select(x => new MeasurementRecord
                    {
                        TimeStamp = x.TimeStamp,
                        SetPoint = x.SetPoint,
                        CurrentAngle = x.CurrentAngle
                    }).ToList()
                };

                sessionRecord.ControllerRecords.Add(yawRecord);
                sessionRecord.ControllerRecords.Add(tiltRecord);

                context.SessionRecords.Add(sessionRecord);
                context.SaveChanges();
            }

            return sessionRecord;
        }

        private static int UpdateSettingsRecord(HelicopterSettings settings, HelicopterModelEntities context)
        {
            var hash = DatabaseManager.GetSHA1Hash(settings.XmlText);
            var settingsRecord = context.SettingsRecords.SingleOrDefault(x => x.Hash == hash);

            if (settingsRecord != null) return settingsRecord.Id;

            settingsRecord = new SettingsRecord
            {
                Created = DateTime.Now,
                Hash = hash,
                Text = settings.XmlText
            };

            context.SettingsRecords.Add(settingsRecord);
            context.SaveChanges();

            return settingsRecord.Id;
        }

        // Creates a collection of DatabaseRow objects from an enumerable of session records
        // These DatabaseRow objects can be easily bound to and displayed from a datagrid
        private static ObservableCollection<DatabaseRow> LoadRecords(IEnumerable<SessionRecord> records)
        {
            var queriedRecords = new ObservableCollection<DatabaseRow>();

            var sessionRecords = records as IList<SessionRecord> ?? records.ToList();

            foreach (var record in sessionRecords)
            {
                var yawRecord = record.ControllerRecords.Single(x => x.MotorType == MotorType.Yaw.ToString());
                var tiltRecord = record.ControllerRecords.Single(x => x.MotorType == MotorType.Tilt.ToString());

                var row = new DatabaseRow
                {
                    Id = record.Id,
                    StartTime = record.StartTime,
                    YawCWProportionalGain = yawRecord.CWProportionalGain,
                    YawCWIntegralGain = yawRecord.CWIntegralGain,
                    YawCWDerivativeGain = yawRecord.CWDerivativeGain,
                    YawCCWProportionalGain = (double)yawRecord.CCWProportionalGain,
                    YawCCWIntegralGain = (double)yawRecord.CCWIntegralGain,
                    YawCCWDerivativeGain = (double)yawRecord.CCWDerivativeGain,
                    TiltCWProportionalGain = tiltRecord.CWProportionalGain,
                    TiltCWIntegralGain = tiltRecord.CWIntegralGain,
                    TiltCWDerivativeGain = tiltRecord.CWDerivativeGain,
                    Relative = RelativeTime.GetTimeAgoString(record.StartTime, DateTime.Now),
                    Comments = record.Comment
                };

                queriedRecords.Add(row);
            }

            return queriedRecords;
        }
    }
}