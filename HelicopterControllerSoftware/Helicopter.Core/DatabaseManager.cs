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
        public static readonly SHA1 HashFunction = SHA1CryptoServiceProvider.Create();
        private const int INT_MaxRowsReturned = 200;
        private static readonly ILog log = LogManager.GetLogger(System.Reflection.MethodBase.GetCurrentMethod().DeclaringType);

        public static string GetSHA1Hash(string str)
        {
            byte[] buffer = Encoding.ASCII.GetBytes(str);
            byte[] hash = HashFunction.ComputeHash(buffer);

            return String.Concat(hash.Select(x => x.ToString("X2")).ToArray()).ToUpper();
        }

        /// <summary>
        /// Gets a collection of DatabaseRow objects by searching through the database using the
        /// specified search paramaters These DatabaseRow objects can be easily bound to and
        /// displayed from a datagrid
        /// </summary>
        /// <param name="records"></param>
        /// <returns>Collection of DatabaseRow objects</returns>
        public static ObservableCollection<DatabaseRow> GetQueriedRecords(bool isSearchingById, int recordId, DateTime date)
        {
            IQueryable<SessionRecord> records;
            ObservableCollection<DatabaseRow> queriedRecords = new ObservableCollection<DatabaseRow>();

            using (var context = new HelicopterModelEntities())
            {
                if (isSearchingById)
                {
                    records = context.SessionRecords.Where(x => x.Id <= recordId).OrderByDescending(x => x.Id).Take(INT_MaxRowsReturned);
                }
                else
                {
                    date = date.AddDays(1);
                    records = context.SessionRecords.Where(x => x.StartTime <= date).OrderByDescending(x => x.Id).Take(INT_MaxRowsReturned);
                }

                queriedRecords = LoadRecords(records);

                return queriedRecords;
            }
        }

        public static bool IsDatabaseAlive()
        {
            try
            {
                string connectionString = "Data Source=AFTABS-DELL; initial catalog=HelicopterModel; persist security info=True; user id=HelicopterControllerSoftware; password=tooEZ2tune; MultipleActiveResultSets=True; App=EntityFramework";

                log.DebugFormat("Connecting to: {0}", connectionString);

                using (var connection = new SqlConnection(connectionString))
                {
                    var query = "select 1";
                    log.DebugFormat("Executing: {0}", query);

                    var command = new SqlCommand(query, connection);

                    connection.Open();
                    log.DebugFormat("SQL Connection successful.");

                    command.ExecuteScalar();
                    log.DebugFormat("SQL Query execution successful.");

                    return true;
                }
            }
            catch (Exception ex)
            {
                log.DebugFormat("Failure: {0}", ex.Message);
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

            if (settingsRecord == null)
            {
                settingsRecord = new SettingsRecord
                {
                    Created = DateTime.Now,
                    Hash = hash,
                    Text = settings.XmlText
                };

                context.SettingsRecords.Add(settingsRecord);
                context.SaveChanges();
            }

            return settingsRecord.Id;
        }

        /// <summary>
        /// Creates a collection of DatabaseRow objects from an enumerable of session records These
        /// DatabaseRow objects can be easily bound to and displayed from a datagrid
        /// </summary>
        /// <param name="records"></param>
        /// <returns>Collection of DatabaseRow objects</returns>
        private static ObservableCollection<DatabaseRow> LoadRecords(IEnumerable<SessionRecord> records)
        {
            var queriedRecords = new ObservableCollection<DatabaseRow>();

            if (records != null || records.Count() != 0)
            {
                foreach (var record in records)
                {
                    var yawRecord = record.ControllerRecords.Single(x => x.MotorType == MotorType.Yaw.ToString());
                    var tiltRecord = record.ControllerRecords.Single(x => x.MotorType == MotorType.Tilt.ToString());

                    var row = new DatabaseRow
                    {
                        Id = record.Id,
                        StartTime = record.StartTime,
                        YawCWProportionalGain = (double)yawRecord.CWProportionalGain,
                        YawCWIntegralGain = (double)yawRecord.CWIntegralGain,
                        YawCWDerivativeGain = (double)yawRecord.CWDerivativeGain,
                        YawCCWProportionalGain = (double)yawRecord.CCWProportionalGain,
                        YawCCWIntegralGain = (double)yawRecord.CCWIntegralGain,
                        YawCCWDerivativeGain = (double)yawRecord.CCWDerivativeGain,
                        TiltCWProportionalGain = (double)tiltRecord.CWProportionalGain,
                        TiltCWIntegralGain = (double)tiltRecord.CWIntegralGain,
                        TiltCWDerivativeGain = (double)tiltRecord.CWDerivativeGain,
                        Relative = RelativeTime.GetTimeAgoString(record.StartTime, DateTime.Now),
                        Comments = record.Comment
                    };

                    queriedRecords.Add(row);
                }
            }

            return queriedRecords;
        }
    }
}