﻿//------------------------------------------------------------------------------
// <auto-generated>
//     This code was generated from a template.
//
//     Manual changes to this file may cause unexpected behavior in your application.
//     Manual changes to this file will be overwritten if the code is regenerated.
// </auto-generated>
//------------------------------------------------------------------------------

namespace Helicopter.Model
{
    using System;
    using System.Data.Entity;
    using System.Data.Entity.Infrastructure;
    
    public partial class HelicopterModelEntities : DbContext
    {
        public HelicopterModelEntities()
            : base("name=HelicopterModelEntities")
        {
        }
    
        protected override void OnModelCreating(DbModelBuilder modelBuilder)
        {
            throw new UnintentionalCodeFirstException();
        }
    
        public virtual DbSet<MeasurementRecord> MeasurementRecords { get; set; }
        public virtual DbSet<SessionRecord> SessionRecords { get; set; }
        public virtual DbSet<SettingsRecord> SettingsRecords { get; set; }
        public virtual DbSet<ControllerRecord> ControllerRecords { get; set; }
    }
}
