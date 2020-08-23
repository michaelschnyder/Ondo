using Microsoft.AspNetCore.Mvc;
using Ondo.Backend.Core.Model;
using Ondo.Backend.Core.Scheduler;
using Ondo.Backend.Core.Services;
using System;
using System.Linq;
using System.Threading.Tasks;

namespace Ondo.Backend.Web.Mvc.Controllers
{
    public class SchedulerController : Controller
    {
        private readonly ISchedulerService _schedulerService;
        private readonly IAirConService _airConService;

        public SchedulerController(ISchedulerService schedulerService, IAirConService airConService)
        {
            _schedulerService = schedulerService;
            _airConService = airConService;
        }

        // GET: Scheduler
        public ActionResult Index()
        {
            var jobSchedules = _schedulerService.GetAllJobScheduleEntities();
            return View(jobSchedules);
        }

        // GET: Scheduler/Create
        public async Task<ActionResult> Create()
        {
            var airCons = await _airConService.GetAirCons();
            ViewBag.SelectableAirCons = airCons.ToList();
            return View();
        }

        // POST: Scheduler/Create
        [HttpPost]
        [ValidateAntiForgeryToken]
        public async Task<ActionResult> Create(JobScheduleEntity jobScheduleEntity)
        {
            try
            {
                var airCons = await _airConService.GetAirCons();
                jobScheduleEntity.PartitionKey = Guid.NewGuid().ToString();
                jobScheduleEntity.RowKey = Guid.NewGuid().ToString();
                jobScheduleEntity.Timestamp = DateTimeOffset.Now;
                jobScheduleEntity.JobType = typeof(AirConJob);
                jobScheduleEntity.Location = airCons.FirstOrDefault(x => x.Id == jobScheduleEntity.AirConId).Location;

                await _schedulerService.InsertOrMergeTableEntityAsync(jobScheduleEntity);
                return RedirectToAction(nameof(Index));
            }
            catch
            {
                return View();
            }
        }

        // GET: Scheduler/Edit/5
        public async Task<ActionResult> Edit(string partitionKey, string rowKey)
        {
            var airCons = await _airConService.GetAirCons();
            ViewBag.SelectableAirCons = airCons.ToList();
            var jobScheduleEntity = await _schedulerService.GetJobsScheduleEntity(partitionKey, rowKey);
            return View(jobScheduleEntity);
        }

        // POST: Scheduler/Edit/5
        [HttpPost]
        [ValidateAntiForgeryToken]
        public async Task<ActionResult> Edit(JobScheduleEntity jobScheduleEntity)
        {
            try
            {
                var airCons = await _airConService.GetAirCons();
                jobScheduleEntity.Location = airCons.FirstOrDefault(x => x.Id == jobScheduleEntity.AirConId).Location;
                await _schedulerService.InsertOrMergeTableEntityAsync(jobScheduleEntity);
                return RedirectToAction(nameof(Index));
            }
            catch
            {
                return View();
            }
        }
        
        // POST: Scheduler/Delete/5
        [HttpPost]
        [ValidateAntiForgeryToken]
        public async Task<ActionResult> Delete(string partitionKey, string rowKey)
        {
            try
            {
                await _schedulerService.DeleteTableEntityAsync(partitionKey, rowKey);
                return RedirectToAction(nameof(Index));
            }
            catch
            {
                return RedirectToAction(nameof(Index));
            }
        }
    }
}