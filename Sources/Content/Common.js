// Function to show a pop-up hint with the alternative text of a hyperlink
function showHint(event) {
    const hint = document.createElement('div');
    hint.className = 'hint'; // Apply the CSS class
    hint.innerText = event.target.getAttribute('alt');

    document.body.appendChild(hint);

    hint.style.left = `${event.pageX + 10}px`;
    hint.style.top = `${event.pageY + 10}px`;
}

// Function to hide the pop-up hint
function hideHint() {
    const hint = document.querySelector('.hint');
    if (hint) {
        hint.remove();
    }
}

// Function to open links of "link buttons" in a new tab
function openLinksInNewTab() {
    const links = document.querySelectorAll('a');
    links.forEach(link => {
        const parentDiv = link.closest('div.link_button');
        if (parentDiv) {
            link.setAttribute('target', '_blank');
        }
    });
}

// Function to open a URL in the same browser window
function openUrl(url) {
    window.location.href = url;
}

// Attach event listeners to all hyperlinks
document.addEventListener('DOMContentLoaded', () => {
    const links = document.querySelectorAll('img[alt]');
    links.forEach(link => {
        link.addEventListener('mouseover', showHint);
        link.addEventListener('mouseout', hideHint);
    });
});

// Attach event listener to open links in a new tab after the DOM is loaded
document.addEventListener('DOMContentLoaded', () => {
    openLinksInNewTab();
});

//  TODO: attach on submit listener. This will encode all text inputs before submitting the form.
/*
$(function() {
    $('form').submit(function(ev) {
       $('form input:text').each(function() {
            $(this).val(encodeURIComponent($(this).val()));
       });
       this.submit();
   });
});
*/
