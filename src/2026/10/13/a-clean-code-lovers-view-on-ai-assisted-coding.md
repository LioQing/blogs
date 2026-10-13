# A Clean Code Lover's View on AI-Assisted Coding (WIP)

2026-10-13

---

First of all, a note on how and why I define myself as a "Clean Code Lover". I think I put emphasis on clean code more than many programmers, but I am not like a "Clean Code Purist".

And as a software engineer, I don't think it is possible to know the entire domain of our application or service most of the time.

So here are some basic rules that I try to follow when I am writing code to let you guys have some expectations on what I will present my view on AI-assisted coding.

## The Structural-Functional Spectrum

I think of each construct (e.g. functions, classes, interfaces, etc.) in a codebase as having a position in a "Structural-Functional" spectrum.

- For more structural constructs, they act as boundaries and classifiers to help with readability. So naturally they are **recurring and often easier to follow the pattern**. Some examples are:
  - The software architecture layers: UI (present visual elements), Service (break down complex workflow into downstream calls), Domain (business logics), Data Access (tailored access functions for business logics to use), Database (SQL migration scripts).
  - The Entity-Component-System architecture's elements: Entities (groups of components), Components (real-time data for operations), Systems (core logics to opreate on data).
  - Or simply a static utility class.
- For more functional constructs, they **act as unique logics and should be more isolated** hence often contained by structural constructs. These are like:
  - Algorithms: Search, Sort, Physics Simulation, Migration Scripts, GPU Shaders.
  - Orchestrator logics: Take layered software architecture, it would be the service layer functions to call certain validation methods and clean up methods in a specific order for specific endpoints.
  - Or simply the main function which runs the event loop for a web app.

Finally, a good codebase should always try to be **consistent** in code organization (though not always possible), hence the existence of design and architecture patterns. I call this the macro-structure of a software.

## My Experience in Using AI for Coding

Okay, that cleans up my point of view on code organization. Let's talk about my experience with AI-assisted coding. I'll just give you a timeline below:

1. 2022 - I started using GitHub Copilot Chat Preview in VSCode, it blew me away with how good it is for small snippets and repetitive codes, though not good for complex reasoning still.
2. 2023 to 2024 - I didn't have much change in using AI for coding, but started working on projects that build agentic behaviors by using trendy topics back then like RAG.
3. 2025 - I got a software engineer job and I started using AI for larger scale and production softwares, though I still mostly use it for test generations and repetitive codes, though sometimes I do use it to generate a first draft of more complicated logics before manually adjusting.
4. 2026 - I finally started trying full-on vibe coded projects, like the [hiking-track-viewer](https://lioqing.com/my-hike-tracker/) I mentioned in [my previous blog](https://lioqing.com/blogs/2026/03/25/how-i-vibe-coded-a-hiking-track-viewer.html).

In the last 2 years, I noticed how fast the current AI-assisted coding capability is improving, and it gave me a lot of thoughts, there are both worries and excitements.

And that's why I am writing this blog right now - I want to record how I feel currently about it. Maybe I'll look back one day and see how different my thoughts have changed, or unchanged.