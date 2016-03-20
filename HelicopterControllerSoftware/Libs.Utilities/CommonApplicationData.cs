using System;
using System.IO;
using System.Reflection;
using System.Security.AccessControl;
using System.Security.Principal;

// derived from:  http://www.codeproject.com/Tips/61987/Allow-write-modify-access-to-CommonApplicationData

namespace Libs.Utilities
{
    /// <summary>
    /// Simplifies the creation of folders in the CommonApplicationData folder
    /// and setting of permissions for all users.
    /// </summary>
    public class CommonApplicationData
    {
        private static readonly string Directory = Environment.GetFolderPath(Environment.SpecialFolder.CommonApplicationData);

        private readonly string applicationFolder;
        private readonly string companyFolder;

        /// <summary>
        /// Initializes a new instance of the <see cref="CommonApplicationData"/> class.
        /// <param name="allUsers">true to allow write/modify to all users; otherwise, false.</param>
        /// </summary>
        public CommonApplicationData(bool allUsers)
        {
            var entryAssembly = Assembly.GetEntryAssembly();

            var productName = ((AssemblyProductAttribute)Attribute.GetCustomAttribute(entryAssembly, typeof(AssemblyProductAttribute))).Product;
            var companyName = ((AssemblyCompanyAttribute)Attribute.GetCustomAttribute(entryAssembly, typeof(AssemblyCompanyAttribute))).Company;

            if (String.IsNullOrEmpty(productName))
            {
                productName = "ProductName";
            }

            if (String.IsNullOrEmpty(companyName))
            {
                companyName = "CompanyName";
            }

            applicationFolder = productName;
            companyFolder = companyName;

            CreateFoldersAndSetAccess(allUsers);
        }

        /// <summary>
        /// Creates a new instance of this class creating the specified company and application folders
        /// if they don't already exist and optionally allows write/modify to all users.
        /// </summary>
        /// <param name="companyFolder">The name of the company's folder (normally the company name).</param>
        /// <param name="applicationFolder">The name of the application's folder (normally the application name).</param>
        /// <remarks>If the application folder already exists then permissions if requested are NOT altered.</remarks>
        public CommonApplicationData(string companyFolder, string applicationFolder)
            : this(companyFolder, applicationFolder, false)
        {
        }

        /// <summary>
        /// Creates a new instance of this class creating the specified company and application folders
        /// if they don't already exist and optionally allows write/modify to all users.
        /// </summary>
        /// <param name="companyFolder">The name of the company's folder (normally the company name).</param>
        /// <param name="applicationFolder">The name of the application's folder (normally the application name).</param>
        /// <param name="allUsers">true to allow write/modify to all users; otherwise, false.</param>
        /// <remarks>If the application folder already exists then permissions if requested are NOT altered.</remarks>
        public CommonApplicationData(string companyFolder, string applicationFolder, bool allUsers)
        {
            this.applicationFolder = applicationFolder;
            this.companyFolder = companyFolder;
            CreateFoldersAndSetAccess(allUsers);
        }

        /// <summary>
        /// Gets the path of the application's data folder.
        /// </summary>
        public string ApplicationFolderPath => Path.Combine(CompanyFolderPath, applicationFolder);

        /// <summary>
        /// Gets the path of the company's data folder.
        /// </summary>
        public string CompanyFolderPath => Path.Combine(Directory, companyFolder);

        /// <summary>
        /// Returns the path of the application's data folder.
        /// </summary>
        /// <returns>The path of the application's data folder.</returns>
        public override string ToString()
        {
            return ApplicationFolderPath;
        }

        private static void SetAccess(string path, FileSystemRights fileSystemRights, InheritanceFlags inheritenceFlags, PropagationFlags propagationFlags, AccessControlType accessControlType)
        {
            var directoryInfo = new DirectoryInfo(path);

            bool modified;
            var directorySecurity = directoryInfo.GetAccessControl();
            SecurityIdentifier securityIdentifier = new SecurityIdentifier(WellKnownSidType.BuiltinUsersSid, null);

            AccessRule rule = new FileSystemAccessRule
            (
                securityIdentifier,
                fileSystemRights,
                inheritenceFlags,
                propagationFlags,
                accessControlType
            );

            directorySecurity.ModifyAccessRule(AccessControlModification.Add, rule, out modified);
            directoryInfo.SetAccessControl(directorySecurity);
        }

        public void SetReadWriteAccessToAllUsers()
        {
            SetReadWriteAccessToAllUsersCompanyFolderPath();
            SetReadWriteAccessToAllUsersApplicationFolderPath();
        }

        private void CreateFoldersAndSetAccess(bool allUsers)
        {
            if (!System.IO.Directory.Exists(CompanyFolderPath))
            {
                System.IO.Directory.CreateDirectory(CompanyFolderPath);
                SetReadWriteAccessToAllUsersCompanyFolderPath();
            }

            if (!System.IO.Directory.Exists(ApplicationFolderPath))
            {
                System.IO.Directory.CreateDirectory(ApplicationFolderPath);

                if (allUsers)
                {
                    SetReadWriteAccessToAllUsersApplicationFolderPath();
                }
            }
        }

        private void SetReadWriteAccessToAllUsersApplicationFolderPath()
        {
            const FileSystemRights fileSystemRights = FileSystemRights.Write | FileSystemRights.ReadAndExecute | FileSystemRights.Modify;
            const InheritanceFlags inheritenceFlags = InheritanceFlags.ContainerInherit | InheritanceFlags.ObjectInherit;

            SetAccess(ApplicationFolderPath, fileSystemRights, inheritenceFlags, PropagationFlags.InheritOnly, AccessControlType.Allow);
        }

        private void SetReadWriteAccessToAllUsersCompanyFolderPath()
        {
            const FileSystemRights fileSystemRights = FileSystemRights.Write | FileSystemRights.ReadAndExecute | FileSystemRights.Modify;
            SetAccess(CompanyFolderPath, fileSystemRights, InheritanceFlags.None, PropagationFlags.None, AccessControlType.Allow);
        }
    }
}