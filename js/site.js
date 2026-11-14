"use strict";

// Theme switcher: cycles through the themes until the user picks one.
const themeImages = [...document.querySelectorAll(".theme-stack img")];
const themeButtons = [...document.querySelectorAll(".chips button")];
let themeTimer = null;

const showTheme = (theme) => {
    themeImages.forEach((image) => image.classList.toggle("active", image.dataset.theme === theme));
    themeButtons.forEach((button) => button.setAttribute("aria-pressed", button.dataset.theme === theme));
};

themeButtons.forEach((button) => {
    button.addEventListener("click", () => {
        clearInterval(themeTimer);
        showTheme(button.dataset.theme);
    });
});

if (!window.matchMedia("(prefers-reduced-motion: reduce)").matches) {
    let themeIndex = 0;
    themeTimer = setInterval(() => {
        themeIndex = (themeIndex + 1) % themeButtons.length;
        showTheme(themeButtons[themeIndex].dataset.theme);
    }, 3500);
}

// Screenshot lightbox.
const lightbox = document.querySelector(".lightbox");
const lightboxImage = lightbox.querySelector("img");

document.querySelectorAll(".gallery button").forEach((button) => {
    button.addEventListener("click", () => {
        lightboxImage.src = button.dataset.full;
        lightboxImage.alt = button.textContent.trim();
        lightbox.showModal();
    });
});

lightbox.addEventListener("click", () => lightbox.close());
