import com.android.build.gradle.AppExtension

plugins {
    alias(libs.plugins.agp.app) apply false
}

fun Project.runCommand(vararg command: String, currentWorkingDir: File = file("./")): String =
    providers.exec {
        workingDir = currentWorkingDir
        commandLine(command.asList())
    }.standardOutput.asText.get().trim()

val gitCommitCount = runCommand("git", "rev-list", "HEAD", "--count").toInt()
val gitCommitHash = runCommand("git", "rev-parse", "--verify", "--short", "HEAD")

// also the soname
val moduleId by extra("inject_env")
val moduleName by extra("Inject Environment")
val verName by extra("v3")
val verCode by extra(gitCommitCount)
val commitHash by extra(gitCommitHash)
val abiList by extra(listOf("arm64-v8a", "armeabi-v7a", "x86", "x86_64"))

val androidMinSdkVersion by extra(26)
val androidTargetSdkVersion by extra(36)
val androidCompileSdkVersion by extra(36)
val androidBuildToolsVersion by extra("36.0.0")
val androidCompileNdkVersion by extra("29.0.14206865")
val androidSourceCompatibility by extra(JavaVersion.VERSION_17)
val androidTargetCompatibility by extra(JavaVersion.VERSION_17)

tasks.register("Delete", Delete::class) {
    delete(rootProject.layout.buildDirectory)
}

fun Project.configureBaseExtension() {
    extensions.findByType(AppExtension::class)?.run {
        namespace = "io.github.a13e300.zygisk.module.sample"
        compileSdkVersion(androidCompileSdkVersion)
        ndkVersion = androidCompileNdkVersion
        buildToolsVersion = androidBuildToolsVersion

        defaultConfig {
            minSdk = androidMinSdkVersion
        }

        compileOptions {
            sourceCompatibility = androidSourceCompatibility
            targetCompatibility = androidTargetCompatibility
        }
    }

}

subprojects {
    plugins.withId("com.android.application") {
        configureBaseExtension()
    }
    plugins.withType(JavaPlugin::class.java) {
        extensions.configure(JavaPluginExtension::class.java) {
            sourceCompatibility = androidSourceCompatibility
            targetCompatibility = androidTargetCompatibility
        }
    }
}
