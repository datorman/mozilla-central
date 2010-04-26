/* Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/publicdomain/zero/1.0/
 */

// This verifies that deleting the database from the profile doesn't break
// anything

const profileDir = gProfD.clone();
profileDir.append("extensions");

function run_test() {
  do_test_pending();
  createAppInfo("xpcshell@tests.mozilla.org", "XPCShell", "1", "1.9.2");

  var dest = profileDir.clone();
  dest.append("addon1@tests.mozilla.org");
  writeInstallRDFToDir({
    id: "addon1@tests.mozilla.org",
    version: "1.0",
    updateURL: "http://localhost:4444/data/test_update.rdf",
    targetApplications: [{
      id: "xpcshell@tests.mozilla.org",
      minVersion: "1",
      maxVersion: "1"
    }],
    name: "Test Addon 1",
  }, dest);

  // Make it look like the detabase was created previously
  Services.prefs.setIntPref("extensions.databaseSchema", 1);

  startupManager(1);

  AddonManager.getAddonByID("addon1@tests.mozilla.org", function(a1) {
    do_check_neq(a1, null);
    do_check_eq(a1.version, "1.0");

    let db = gProfD.clone();
    db.append("extensions.sqlite");
    do_check_true(db.exists());
    do_check_true(db.fileSize > 0);

    do_test_finished();
  });
}
