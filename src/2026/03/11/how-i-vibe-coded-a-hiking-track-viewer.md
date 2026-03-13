# How I Vibe Coded a Hiking Track Viewer

2026-03-11

---

I recently decided to try out full-on vibe coding on a personal project where I want a visualization of my weekly hikes. For years, I've been hiking around where I live every weekend as a way to refresh myself from the repetitive day jobs, and I have been tracking it with [Google Fit](https://www.google.com/fit/).

One day, I found out that I can export and download the data using [Google Takeout](https://takeout.google.com/), and so I had the idea to make a map viewer that displays my hiking tracks over time.

However, since I am now a full-time software engineer, I don't really want to spend that much time on coding all day, especially for a personal project like this. That's when I decided I will finally try letting LLMs to code everything for me.

> [!NOTE]
>
> Since this is a rather small project, I did not use any advanced LLM prompting techniques like tuning instructions or skills.
>
> I also did not do everything in one session, I intentionally broke the conversations into different sections for specific reason explained later and for avoiding overflowing the context window.

And it did work out for me, it only took me around 3 evenings prompting [GitHub Copilot Pro](https://github.com/github-copilot/pro). I am pretty satisfied with the result.

**Insert video of the website**

In the process, I applied a lot of ideas that I gained from previously coding with LLMs, but I also learnt something new about 
letting AI code an entire project. I'll share them one by one in the following sections.

## Data Source

Before talking about the website, I'd like to talk about where it all started. It all started with where I have my hiking track data.

I hike with Google Fit tracking my progress on my phone every weekend. While the UI of Google Fit is fine and does display useful information, I want to have a more personalized view of my progress.

### Where to download the data

That is why when I found out I can [download the Google Fit data from Google Takeout](https://takeout.google.com/takeout/custom/fit), I decided to make an app or website to display it in a cooler way.

![Download Google Fit Data](download-google-fit-data.png)
*Google Fit privacy page where user can export their fit data.*

One unfortunate thing is that there is no option to choose a date range or activity type to download your fitness data, so for me, each time I download the data, I am downloading all my activities including hikes, walks, cyclings, etc. since I started using Google Fit in 2018.

### The file format

Before I even start looking at the data, I asked Gemini (my choice of LLM for generic questions) how to extract timestamps and positions from the Google Fit data. This is when I learnt about the [Training Center XML (TCX)](https://en.wikipedia.org/wiki/Training_Center_XML) file format.

It is a file format designed specifically for tracking exercises by Garmin in 2007. It is basically just an XML file with standardized entries.

With it being an established format, can now more confidently let codign agents to help me handle the technical details.

## The Programming Languages and The (Lack of) Framework

I think this project is a pretty typical small-scale personal project for me, great for showing how I decide what tools to use for the tasks.

### The languages

When it comes to these small personal projects, I have already established a simple rule of choosing what language to use and collaborate with coding agents:

- For simple tasks, use established and popular languages.
  - Obviously the choice for building a website is just JavaScript.
- For complex tasks (which I may have to chime in and make changes myself instead of handing off everything to the agent), use languages I am familiar with.
  - For the data extraction and cleaning, I chose Rust, where I want to make sure the ingested data for the website is correct.
  - Most of the time, it is just Rust.

### The vanilla web development experience

You may think that using a framework like [React](https://react.dev/) would be a great fit for it (which I also have experience in), but honestly I have almost never considered using React and instead just do vanilla HTML + CSS + JS because of the following reasons:

- LLMs have been trained on plenty of code examples for vanilla HTML + CSS + JS given that [web standards](https://www.w3.org/standards/) are relatively stable compared to flashy frameworks.
- It is easy to deploy to GitHub Pages where I host all of my website frontends.
- Setting up a project and compiling/building my code is pretty time consuming.
- I just don't like how state is managed in most frameworks.

Unless I absolutely want certain features or libraries from the ecosystem of the framework (especially for the beautiful animations and asthetics), I would prefer to [just f**king use HTML](https://justfuckingusehtml.com/).

## The Collaboration with GitHub Copilot

After brief consulting with Gemini to confirm the feasibility of reconstructing my hiking tracks using the Google Fit data, I decided to dive straight into building it.

After all, the only way to start something is to just do it.

### Ingesting the data

At first, I searched up a crate for deserializing tcx format - [tcx](https://crates.io/crates/tcx), and told copilot to start with that. Copilot tried using that crate to deserialize my file.

However, I got the following result when trying to parse one of my hiking TCX files.

```
ParseIntError: invalid digit found in string
```

I suspect this is due to the crate being updated over 4 years ago, which means it is probably outdated if the TCX format has been updated or Google Fit uses a custom TCX format.

So I had to find another solution.

I went ahead and took a look at the dependencies of the tcx crate, I found that it just uses [serde](https://crates.io/crates/serde) with some XML serde deserializer crate. I decided to give copilot a try on just writing a custom TCX deserializer by using serde and whatever XML serde deserializer it wants to use.

After a couple of prompts (to let copilot know that it can test using my TCX file and to further specify the format I want), I was able to extract the timestamp, latitude, and longitude from each hike!

```
=== C:\Users\s2010\Downloads\takeout-20260221T061429Z-3-001\Takeout\Fit\Activities\2026-02-08T07_43_23.438+08_00_PT1H19M30.202S_Hiking.tcx ===    
  1770507805634 -> 22.283260345458984,114.13562774658203
  1770507806472 -> 22.283279418945313,114.13563537597656
  1770507807250 -> 22.283302307128906,114.1356430053711
  ...
```

After successfully extracting the timestamp, latitude, and longitude, I just need to write it to a file for the website to consume. I decided to use CSV as it should be the most straightforward solution.

And it is indeed the most straightforward solution, I opened a new chat session and copilot was able to get it done in just one prompt. I simply told it to write the result to a headerless CSV file with the columns being the timestamp in UNIX seconds, and latitude and longitude in high precision.

After that, I just told copilot to do some chores like refactoring into functions and adding CLI arguments using [clap](https://crates.io/crates/clap).

Additionally, I did some documentations myself for the CLI arguments to keep my future self in context in case I want to further develop or reference the code in future.

### Cleaning the data

While I added the cleaner at almost the end of the project, I want to group it to this section because the process was similar to extracting the data.

When I was checking out my hiking tracks in the website, I realized that sometimes due to me forgetting to complete the tracking in Google Fit app, my positions when I was on railway would get recorded. It makes me look like I am flying across the city.

**Insert video of me flying across the city**

So I created another Rust project for removing unwanted data points using boolean expression filter rules.

I looked up expression evaluation crates, and found [evalexpr](https://docs.rs/evalexpr/latest/evalexpr/). It was exactly what I wanted, it can evaluate to boolean, substitude variables, and even uses comments to document my filter rules.

The implementation process was very straightforward as well, the chat session only took me 2 prompts. First one for telling copilot to use the evalexpr crate to let user use the extracted data with their corresponding column names in a single column CSV file to filter data points from the extracted data. Second one for telling copilot to remove the header (for some reason it decided to add one named 'expression').

And the result is the following expected format.

```
!(timestamp > 1767456000000 && timestamp < 1767495660000) // 2026-01-04 before 11:01: tracking bugged at the start
!(timestamp > 1769925420000 && timestamp < 1769961600000) // 2026-02-01 after 13:57: forgot to turn off tracking in MTR
```

Apart from the extra data points, I also realized that sometimes due to me forgetting to resume the tracking in the Google Fit app, the data points are missing in those sections.

**Insert image of tracks with missing data**

It certainly makes the visualization a lot more unpleasant to watch, so I had to find a solution.

While Google Fit itself uses some kind of weird algorithm to try to estimate the missing positions in between, the result is... kind of inaccurate (It is very inaccurate to be honest).

**Insert image of Google Fit with missing points**

So at the end I decided to just let copilot to implement a manual solution - let the user enter the timestamp in 